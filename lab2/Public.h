#pragma once
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SLOTS 5        // 缓冲区槽位数
#define DATA_SIZE 27           // 数据单元字节数
#define SEM_KEY_MUTEX 0x1111   // 互斥信号量键值
#define SEM_KEY_EMPTY 0x1113   // 空槽位信号量
#define SEM_KEY_FULL 0x1112    // 满槽位信号量
#define SHM_KEY 0x1246         // 共享内存键值

typedef union {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} SemArg;


typedef struct {
    char storage[BUFFER_SLOTS][DATA_SIZE];
    int slot_status[BUFFER_SLOTS]; // 0=可写 1=可读
} SharedBuffer;


//信号量操作实现 
int init_semaphore(int key, int init_val) {
    int semid = semget(key, 1, IPC_CREAT | 0666);  // 创建/获取信号量[9](@ref)
    if (semid == -1) {
        perror("semget failed");
        return -1;
    }

    SemArg arg;
    arg.val = init_val;
    if (semctl(semid, 0, SETVAL, arg) == -1) {  // 初始化信号量值[10](@ref)
        perror("semctl SETVAL failed");
        return -1;
    }
    return semid;
}

int sem_p(int sem_id) {
    struct sembuf op = {
        .sem_num = 0,
        .sem_op = -1,  // P操作减1
        .sem_flg = SEM_UNDO  // 异常退出自动释放[10](@ref)
    };
    return semop(sem_id, &op, 1) == 0 ? 0 : -1;
}

int sem_v(int sem_id) {
    struct sembuf op = {
        .sem_num = 0,
        .sem_op = 1,   // V操作加1
        .sem_flg = SEM_UNDO
    };
    return semop(sem_id, &op, 1) == 0 ? 0 : -1;
}

void cleanup_sem(int sem_id) {
    SemArg dummy;
    semctl(sem_id, 0, IPC_RMID, dummy);  // 删除信号量集[9](@ref)
}

// 共享内存操作实现 
SharedBuffer* attach_shared_mem() {
    int shmid = shmget(SHM_KEY, sizeof(SharedBuffer), IPC_CREAT | 0666);  // 创建共享内存[6](@ref)
    if (shmid == -1) {
        perror("shmget failed");
        return NULL;
    }

    void* ptr = shmat(shmid, NULL, 0);  // 附加到进程地址空间[7](@ref)
    if (ptr == (void*)-1) {
        perror("shmat failed");
        return NULL;
    }

    // 初始化缓冲区状态
    SharedBuffer* buf = (SharedBuffer*)ptr;
    for (int i = 0; i < BUFFER_SLOTS; ++i) {
        buf->slot_status[i] = 0;  // 初始为空闲状态[1](@ref)
    }
    return buf;
}

void detach_shared_mem(SharedBuffer* ptr) {
    if (shmdt(ptr) == -1) {  // 分离共享内存[7](@ref)
        perror("shmdt failed");
    }
}
