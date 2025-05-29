#include "Public.h"
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void producer_delay() {
    struct timespec ts = { 
        .tv_sec = 0,
        .tv_nsec = (rand() % 5000 + 1000) * 1000000 // 1-6ms随机延迟
    };
    nanosleep(&ts, NULL);
}

void write_product(SharedBuffer* buf, int slot, const char* data) {
    memcpy(buf->storage[slot], data, DATA_SIZE);
    buf->slot_status[slot] = 1;  // 标记为已填充
    printf("[Producer] Write data to slot %d: %.5s...\n", slot, data);
}

int main() {
    SharedBuffer* buffer = attach_shared_mem();
    int mutex = init_semaphore(SEM_KEY_MUTEX, 1);
    int empty = init_semaphore(SEM_KEY_EMPTY, BUFFER_SLOTS);
    int full = init_semaphore(SEM_KEY_FULL, 0);
    
    for(int cycle = 0; ; ++cycle) {
        char data[DATA_SIZE];
        int fd = open("./producer_source.txt", O_RDONLY);
        read(fd, data, sizeof(data));
        close(fd);

        sem_p(empty);  // 等待空槽位
        sem_p(mutex);  // 进入临界区
        
        for(int i = 0; i < BUFFER_SLOTS; ++i) {
            if(buffer->slot_status[i] == 0) {
                write_product(buffer, i, data);
                break;
            }
        }
        
        sem_v(mutex); // 退出临界区
        sem_v(full);  // 增加满槽计数
        producer_delay();
    }
    
    cleanup_sem(mutex);
    cleanup_sem(empty);
    cleanup_sem(full);
    detach_shared_mem(buffer);
    return 0;
}