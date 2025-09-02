#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFNUM 5
#define BUFSIZE 100
#define MYKEY 4567

typedef struct BufferPool
{
    char Buffer[BUFNUM][BUFSIZE];
    int stat[BUFNUM];   // 0 表示可以写   1 表示可以读
} BufferPool;

// 信号量操作函数声明
int init_sem(int sem_id, int init_value);
int semaphore_p(int sem_id, short sem_no);
int semaphore_v(int sem_id, short sem_no);
int del_sem(int sem_id);

// 信号量联合体（兼容System V）
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};


int init_sem(int sem_id, int init_value)
{
    union semun sem_union;
    sem_union.val = init_value;
    if (semctl(sem_id, 0, SETVAL, sem_union) == -1)
    {
        perror("Initialize semaphore");
        return -1;
    }
    return 1;
}

int semaphore_p(int sem_id, short sem_no)
{
    struct sembuf sem_b;
    sem_b.sem_num = sem_no; // 信号量集中信号量编号
    sem_b.sem_op = -1;      // P操作，每次分配1个资源
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        perror("semaphore_p failed");
        return 0;
    }
    return 1;
}

int semaphore_v(int sem_id, short sem_no)
{
    struct sembuf sem_b;
    sem_b.sem_num = sem_no; // 信号量集中信号量编号
    sem_b.sem_op = 1;       // V操作，每次释放1个资源
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1)
    {
        perror("semaphore_v failed");
        return 0;
    }
    return 1;
}

int del_sem(int sem_id)
{
    union semun sem_union;
    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
    {
        perror("Delete semaphore");
        return -1;
    }
    return 1;
}
