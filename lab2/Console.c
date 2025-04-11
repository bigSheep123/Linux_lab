#include "Buffer.h"

char* name[]={"Roieee","Pite174","3mz","gusenpai","boring","xx","jj","qq","feiju","MingYue","bolin"};

void SubprocessFactory(const char* path,const char* key,const char* name){
    pid_t pid=vfork();
    if(pid<0){
        perror("fork error");
        exit(1);
    }
    else if(pid==0){
        /// @todo 告诉Consumer共享内存在哪里
        execl(path,path,key,name,NULL);
        perror("execl failed");
    }
    return;
}

int main(int argc,char* argv[]){
    key_t key=ftok(argv[0],0);//生成key
    printf("%s\n",argv[0]);
    GetSharedMemory(key,true);
    #define PROCESS_NUM 4
    int name_cnt=0;
    for(int i=0;i<PROCESS_NUM;++i){
        SubprocessFactory("./Producer",argv[0],name[name_cnt++]);
        SubprocessFactory("./Consumer",argv[0],name[name_cnt++]);
        sleep(1);
    }
    #undef PROCESS_NUM
    pause();
    return 0;
}