#include <assert.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>

#define VIDEO_DEVICE "/dev/video0"  
#define IMG_WIDTH 640
#define IMG_HEIGHT 480
#define NUM_BUFFERS 4               
#define CAPTURE_TIMEOUT 3           

typedef struct {
    void *start;
    size_t length;
} Buffer;

Buffer *buffers = NULL;
unsigned int n_buffers = 0;

// 设置视频格式
void set_video_format(int fd, struct v4l2_format *fmt) {
    memset(fmt, 0, sizeof(*fmt));
    fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt->fmt.pix.width = IMG_WIDTH;
    fmt->fmt.pix.height = IMG_HEIGHT;
    fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;
    
    if (ioctl(fd, VIDIOC_S_FMT, fmt) < 0) {
        perror("Failed to set video format");
        exit(EXIT_FAILURE);
    }
    
    // 验证实际设置的格式
    if (fmt->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
        fprintf(stderr, "Device doesn't support YUYV format\n");
        exit(EXIT_FAILURE);
    }
}

// 请求并映射内存缓冲区
void init_mmap(int fd) {
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    
    req.count = NUM_BUFFERS;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("Failed to request buffers");
        exit(EXIT_FAILURE);
    }
    
    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory\n");
        exit(EXIT_FAILURE);
    }
    
    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        perror("Failed to allocate buffer memory");
        exit(EXIT_FAILURE);
    }
    
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("Failed to query buffer");
            exit(EXIT_FAILURE);
        }
        
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length, 
                                      PROT_READ | PROT_WRITE, MAP_SHARED,
                                      fd, buf.m.offset);
        
        if (buffers[n_buffers].start == MAP_FAILED) {
            perror("Failed to mmap buffer");
            exit(EXIT_FAILURE);
        }
        
        // 将缓冲区加入队列
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("Failed to queue buffer");
            exit(EXIT_FAILURE);
        }
    }
}

// 启动视频流
void start_streaming(int fd) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("Failed to start streaming");
        exit(EXIT_FAILURE);
    }
}

// 停止视频流
void stop_streaming(int fd) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("Failed to stop streaming");
    }
}

// 捕获一帧图像
void capture_frame(int fd, const char *filename) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    
    // 使用poll等待数据就绪
    struct pollfd pfd = {fd, POLLIN, 0};
    int ret = poll(&pfd, 1, CAPTURE_TIMEOUT * 1000);
    
    if (ret < 0) {
        perror("Error during poll");
        return;
    } else if (ret == 0) {
        fprintf(stderr, "Capture timeout\n");
        return;
    }
    
    // 取出已填充数据的缓冲区
    if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
        perror("Failed to dequeue buffer");
        return;
    }
    
    // 保存图像到文件
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Failed to open output file");
        return;
    }
    
    fwrite(buffers[buf.index].start, buf.bytesused, 1, fp);
    fclose(fp);
    printf("Captured frame saved to %s (%zu bytes)\n", filename, buf.bytesused);
    
    // 将缓冲区重新加入队列
    if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
        perror("Failed to requeue buffer");
    }
}

// 清理资源
void cleanup(int fd) {
    // 取消映射缓冲区
    for (unsigned int i = 0; i < n_buffers; ++i) {
        if (buffers[i].start && munmap(buffers[i].start, buffers[i].length) < 0) {
            perror("Failed to unmap buffer");
        }
    }
    free(buffers);
    // 关闭设备
    if (fd >= 0) close(fd);
}

int main() {
    int fd = -1;
    struct v4l2_format fmt;
    struct v4l2_capability cap;
    
    // 打开视频设备
    fd = open(VIDEO_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open video device");
        exit(EXIT_FAILURE);
    }
    
    // 查询设备能力
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("Failed to query device capabilities");
        cleanup(fd);
        exit(EXIT_FAILURE);
    }
    
    printf("Driver: %s\nCard: %s\nBus: %s\nVersion: %u.%u.%u\n",
           cap.driver, cap.card, cap.bus_info,
           (cap.version >> 16) & 0xFF,
           (cap.version >> 8) & 0xFF,
           cap.version & 0xFF);
    
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "Device does not support video capture\n");
        cleanup(fd);
        exit(EXIT_FAILURE);
    }
    
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "Device does not support streaming\n");
        cleanup(fd);
        exit(EXIT_FAILURE);
    }
    
    // 设置视频格式
    set_video_format(fd, &fmt);
    printf("Set format: %dx%d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
  
    init_mmap(fd);
    start_streaming(fd);
  
    capture_frame(fd, "capture.yuv");
    
    // 停止流并清理
    stop_streaming(fd);
    cleanup(fd);
    
    return 0;
}