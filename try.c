// #include <stdio.h>
// #include <sys/shm.h>
// #include <sys/sem.h>

// union semun {
//     int val;
//     struct semid_ds *buf;
//     unsigned short* array;
// };

// int sem_init()
// {
//     int semid = semget(1234, 0, IPC_CREAT| 0666);
//     union semun arg;
//     arg.val = 1;
//     semctl(semid,0,SETVAL,arg);
//     return semid;
// }

// void wait_sem(int semid) //p 操作
// {
//     struct sembuf op = {0,-1,SEM_UNDO};
//     semop(semid,&op,1);
// }

// void post_sem(int semid) // v 操作
// {
//     struct sembuf op = {0,-1,SEM_UNDO};
//     semop(semid, &op, 1);
// }

// int main()
// {
//     int semid = sem_init();
//     wait_sem(semid);
//     post_sem(semid);

//     semctl(semid,0,IPC_RMID);
// }

// #include <sys/shm.h>
// #include <stdio.h>

// int main()
// {
//     int size;
//     int shmid = shmget(0123,size,IPC_CREAT|0666);
//     int* a = ( int*) shmat(shmid,NULL,0);
//     shmdt(a);
//     shmctl(shmid,IPC_RMID,0);
//     return 0;
// }

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>

// int main()
// {
//     int size;
//     int shmid = shmget(1234,size,0666 | IPC_CREAT);
//     int* addr = (int*) shmat(shmid,NULL,0);
//     shmdt(addr);
//     shmctl(shmid,IPC_RMID,0);
//     return 0;
// }

// union semun {
//     int val;
//     struct semid_ds *buf;
//     unsigned short* array;
// };

// int init_sem()
// {
//     int semid = semget(0123,1,IPC_CREAT | 0666);
//     union semun arg;
//     semctl(semid,0,SETVAL,arg);
//     return semid;
// }

// void sem_wait(int semid)
// {
//     struct sembuf op = {0,-1,SEM_UNDO};
//     semop(semid,&op,1);
// }

// void sem_post(int semid)
// {
//     struct sembuf op = {0,1,SEM_UNDO};
//     semop(semid, &op, 1);
// }

// int main()
// {
//     int semid = init_sem();
//     sem_wait(semid);
//     sem_post(semid);
//     semctl(semid,0,IPC_RMID);
//     return 0;
// }