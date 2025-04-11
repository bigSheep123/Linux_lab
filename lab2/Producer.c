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
        printf("Producer:%s Waiting...\n",argv[2]);
        int ready=pop_front(ready2produce,ptr->mem);
        if(0<=ready&&ready<BufferSize){
            // sleep(3);
            for(int j=0;j<31;++j)
                ptr->mem[ready].data[j]='A'+rand()%26;
            ptr->mem[ready].data[31]=0;
            printf("Producer:%s Produced:%s\n",argv[2],ptr->mem[ready].data);
            push_back(ready2consume,ptr->mem,ready);
        }
        else{
            printf("%d\n",ready);
            printf("Error Occured!\n");
            exit(-1);
        }
        // sleep(2);
    }
}