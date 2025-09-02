#include "Public.h"

#define FILENAME "./comsumerN.txt"

int main() {
    // 获取三个信号量
    int mutex_sem = semget(MYKEY, 1, 0666);      // 互斥锁
    int empty_sem = semget(MYKEY+1, 1, 0666);    // 空槽位
    int full_sem = semget(MYKEY+2, 1, 0666);     // 数据槽位

    // 获取共享内存
    int shm_id = shmget(MYKEY+3, sizeof(BufferPool), 0666);
    BufferPool *bufPool = (BufferPool*)shmat(shm_id, NULL, 0);

    while (1) {
        semaphore_p(full_sem, 0);   // 等待数据槽位（有数据可读）[1](@ref)
        semaphore_p(mutex_sem, 0);   // 获取互斥锁

        // 寻找可读缓冲区消费
        for (int i = 0; i < BUFNUM; i++) {
            if (bufPool->stat[i] == 1) {
                write(STDOUT_FILENO, bufPool->Buffer[i], BUFSIZE);  // 修复：写入完整数据
                bufPool->stat[i] = 0;  // 标记为空闲
                break;
                
            }
        }

        semaphore_v(mutex_sem, 0);  // 释放互斥锁
        semaphore_v(empty_sem, 0);  // 增加空槽位（通知生产者）[7](@ref)
    }

    shmdt(bufPool);
    return 0;
}