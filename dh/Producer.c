#include "Public.h"


#define FILENAME "./producerN.txt"

int main() {
    int fd = open(FILENAME, O_RDONLY);
    if (fd == -1) {
        perror("open producer file failed");
        exit(EXIT_FAILURE);
    }

    // 创建三个信号量：互斥锁(mutex_sem)、空槽位(empty_sem)、数据槽位(full_sem)
    int mutex_sem = semget(MYKEY, 1, 0666 | IPC_CREAT);   // 互斥锁，初始值=1
    int empty_sem = semget(MYKEY+1, 1, 0666 | IPC_CREAT); // 空槽位，初始值=BUFNUM
    int full_sem = semget(MYKEY+2, 1, 0666 | IPC_CREAT);  // 数据槽位，初始值=0

    init_sem(mutex_sem, 1);
    init_sem(empty_sem, BUFNUM);
    init_sem(full_sem, 0);

    // 创建并初始化共享内存
    int shm_id = shmget(MYKEY+3, sizeof(BufferPool), 0666 | IPC_CREAT);
    BufferPool *bufPool = (BufferPool*)shmat(shm_id, NULL, 0);
    memset(bufPool, 0, sizeof(BufferPool));  // 关键：初始化所有stat为0（可写）

    char buffer[5];
    while (read(fd, buffer, sizeof(buffer)) > 0) {
        semaphore_p(empty_sem, 0);  // 等待空槽位 [1,7](@ref)
        semaphore_p(mutex_sem, 0);  // 获取互斥锁

        // 寻找空闲缓冲区写入
        for (int i = 0; i < BUFNUM; i++) {
            if (bufPool->stat[i] == 0) {
                strncpy(bufPool->Buffer[i], buffer, sizeof(buffer));
                bufPool->stat[i] = 1;  // 标记为可读
                break;
            }
        }

        semaphore_v(mutex_sem, 0);  // 释放互斥锁
        semaphore_v(full_sem, 0);   // 增加数据槽位（通知消费者）[2](@ref)
    }

    // 资源清理（仅生产者负责删除）
    shmdt(bufPool);
    close(fd);
    shmctl(shm_id, IPC_RMID, 0);   // 删除共享内存
    del_sem(mutex_sem);
    del_sem(empty_sem);
    del_sem(full_sem);
    return 0;
}