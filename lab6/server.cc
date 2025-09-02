#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define BUFFER_SIZE 1024
#define DEFAULT_PORT 8080
#define WEB_ROOT "/var/www" // 保存文件

void safe_close(int fd) {
    if (fd >= 0) close(fd);
}

void send_error(int fd, int status, const char* message) {
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 %d %s\r\nContent-Type: text/plain\r\n\r\n%s",
             status, message, message);
    send(fd, response, strlen(response), 0);
}

void serve_file(int fd, const char* path, const char* content_type) {
    struct stat file_stat;
    if (stat(path, &file_stat) < 0) {
        send_error(fd, 404, "Not Found");
        return;
    }

    int file_fd = open(path, O_RDONLY);
    if (file_fd < 0) {
        send_error(fd, 403, "Forbidden");
        return;
    }

    char headers[BUFFER_SIZE];
    snprintf(headers, sizeof(headers),
             "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n",
             content_type, file_stat.st_size);

    send(fd, headers, strlen(headers), 0);
    sendfile(fd, file_fd, nullptr, file_stat.st_size);
    safe_close(file_fd);
}

const char* get_content_type(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return "text/plain";
    
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    
    return "text/plain";
}

// 处理HTTP请求
void handle_http(int fd) {
    char buffer[BUFFER_SIZE], method[16], url[256], version[16];
    memset(buffer, 0, BUFFER_SIZE);

    // 接收请求数据
    ssize_t bytes_read = recv(fd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_read <= 0) {
        perror("recv failed");
        safe_close(fd);
        return;
    }

    if (sscanf(buffer, "%15s %255s %15s", method, url, version) != 3) {
        send_error(fd, 400, "Bad Request");
        safe_close(fd);
        return;
    }

    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", WEB_ROOT, 
             (strcmp(url, "/") == 0) ? "/index.html" : url);

    serve_file(fd, full_path, get_content_type(full_path));
    safe_close(fd);
}

int main(int argc, char* argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return EXIT_FAILURE;
    }

    // 设置SO_REUSEADDR避免"Address already in use"错误
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        safe_close(sockfd);
        return EXIT_FAILURE;
    }

    if (listen(sockfd, 10) < 0) {
        perror("listen failed");
        safe_close(sockfd);
        return EXIT_FAILURE;
    }

    printf("HTTP server running on port %d\n", port);

    while (true) {
        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        // 多进程处理并发请求
        pid_t pid = fork();
        if (pid == 0) { // 子进程
            close(sockfd);
            handle_http(client_fd);
            exit(EXIT_SUCCESS);
        } else if (pid > 0) { // 父进程
            close(client_fd);
        } else {
            perror("fork failed");
            close(client_fd);
        }
    }

    safe_close(sockfd);
    return EXIT_SUCCESS;
}