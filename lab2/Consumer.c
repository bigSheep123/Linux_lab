#include "Buffer.h"
#include <assert.h>
#include <time.h>

int main(int argc,char* argv[]){
    assert(argc==3);
    key_t key=ftok(argv[1],0);
    struct DATAPACK* ptr=GetSharedMemory(key,false);
    struct BuffQueue* ready2produce=&ptr->write;
    struct BuffQueue* ready2consume=&ptr->read;
    srand(time(NULL));
    while(true){
        printf("Consumer:%s Waiting...\n",argv[2]);
        int ready=pop_front(ready2consume,ptr->mem);
        if(0<=ready&&ready<BufferSize){
            // “消化”不良，模拟长时间运行
            // sleep(2);
            printf("Consumer:%s Consumed:%s\n",argv[2],ptr->mem[ready].data);
            push_back(ready2produce,ptr->mem,ready);
        }
        else{
            printf("%d\n",ready);
            printf("Error Occured!\n");
            exit(-1);
        }
        // sleep(2);
    }
}