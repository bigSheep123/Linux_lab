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

void consumer_delay() {
    struct timespec ts = {
        .tv_sec = 0, 
        .tv_nsec = (rand() % 8000 + 2000) * 1000000 // 2-10ms随机延迟
    };
    nanosleep(&ts, NULL);
}

void read_product(SharedBuffer* buf, int slot, char* output) {
    memcpy(output, buf->storage[slot], DATA_SIZE);
    buf->slot_status[slot] = 0;  // 标记为空闲
    printf("[Consumer] Read data from slot %d: %.5s...\n", slot, output);
}

int main() {
    SharedBuffer* buffer = attach_shared_mem();
    int mutex = init_semaphore(SEM_KEY_MUTEX, 1);
    int full = init_semaphore(SEM_KEY_FULL, 0);
    int empty = init_semaphore(SEM_KEY_EMPTY, BUFFER_SLOTS);

    while(1) {
        sem_p(full);   // 等待满槽位
        sem_p(mutex);  // 进入临界区
        
        for(int i = 0; i < BUFFER_SLOTS; ++i) {
            if(buffer->slot_status[i] == 1) {
                char data[DATA_SIZE];
                read_product(buffer, i, data);
                
                int fd = open("./consumer_output.txt", O_WRONLY|O_CREAT, 0644);
                write(fd, data, sizeof(data));
                close(fd);
                break;
            }
        }
        
        sem_v(mutex);  // 退出临界区
        sem_v(empty);  // 增加空槽计数
        consumer_delay();
    }
    
    detach_shared_mem(buffer);
    return 0;
}