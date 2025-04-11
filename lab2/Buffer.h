#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <semaphore.h>
#define BufferSize 10
struct BuffNode{
    char data[32];
    int nxt;
};
struct BuffQueue{
    int head,tail;
    sem_t ready;//需要互斥访问
    sem_t size;//需要有
};

void Init(struct BuffQueue* que){
    que->head=-1;
    que->tail=-1;
    sem_init(&que->ready,1,1);
    sem_init(&que->size,1,0);
}

void push_back(struct BuffQueue* que,struct BuffNode* addr,int ind){
    sem_wait(&que->ready);//producer 卡在 ready2consume 的这里
    if(que->tail!=-1){
        addr[que->tail].nxt=ind;
        que->tail=ind;
    }
    else{
        que->head=ind;
        que->tail=ind;
    }
    addr[ind].nxt=-1;
    sem_post(&que->size);
    sem_post(&que->ready);
}

int pop_front(struct BuffQueue* que,struct BuffNode* addr){
    sem_wait(&que->size);
    sem_wait(&que->ready);
    // printf("SOMEBODY IS IN!\n");
    int ret=que->head;
    if(que->head!=que->tail)
        que->head=addr[que->head].nxt;
    else{
        que->head=-1;
        que->tail=-1;
    }
    addr[ret].nxt=-1;
    sem_post(&que->ready);
    // printf("SOMEBODY POST!\n");
    return ret;
}

struct DATAPACK
{
    struct BuffQueue read,write;
    struct BuffNode mem[BufferSize];
};


struct DATAPACK* GetSharedMemory(key_t key,bool create){
    int shmid;
    if(create)
        shmid=shmget(key,sizeof(struct DATAPACK),IPC_CREAT|0666);//创建共享内存
    else
        shmid=shmget(key,sizeof(struct DATAPACK),0666);
    if(shmid==-1){
        perror("shmget failed");
        exit(1);
    }
    void *shared=shmat(shmid,NULL,0);
    if(shared==(void*)-1){
        perror("shmat failed");
        exit(1);
    }
    struct DATAPACK* p=(struct DATAPACK*)shared;
    if(create){
        struct BuffQueue* read=&p->read;
        struct BuffQueue* write=&p->write;
        Init(read);
        write->head=0;
        for(int i=0;i<BufferSize-1;i++)
            p->mem[i].nxt=i+1;
        p->mem[BufferSize-1].nxt=-1;
        write->tail=BufferSize-1;
        sem_init(&write->size,1,BufferSize);
        sem_init(&write->ready,1,1);
        // int value;
        // sem_getvalue(&write->ready,&value);
        // printf("%d\n",value),
        // sem_getvalue(&write->size,&value);
        // printf("%d\n",value);
        // printf("%p\n",write->head);
    }
    return p;
}