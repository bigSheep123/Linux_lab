#include<stdio.h>
#include <sys/ipc.h>
#include<sys/sem.h>
#define MYKEY 0x1a2a

// sudo ipcrm -s id
// sudo ipcrm -m id
int main()
{
    int semid;
    semid = semget(MYKEY, 1, 0666|IPC_CREAT);
    printf("semid = %d\n",semid);

    

    int ret = semctl(semid,0,IPC_RMID);
    return 0;
}

