// #include <stdio.h>
// #include <pthread.h>
// #include <time.h>

// void* action(void* args)
// {
//     int e = *(int*) args;

//     // 哲学家编号
//     if( e % 2 == 0)
//     {

//     }
//     else if( e % 2 == 1)
//     {

//     }
// }

// // 全局锁
// pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

// int main()
// {
//     // 锁的初始化

//     // 局部锁的初始化
//     pthread_mutex_t localmut;
//     pthread_mutex_init(&localmut,NULL);

//     pthread_t t1;
//     int a = 10;
//     pthread_create(&t1,NULL,action,&a);

//     pthread_join(t1,NULL);

//     pthread_mutex_destroy(&localmut);

//     pthread_mutex_lock(&mut);
//     int ret = pthread_mutex_trylock(&mut);
//     pthread_mutex_unlock(&mut);
// }


/// lab 3
/// Dir  

// #include <stdio.h>
// #include <unistd.h>
// #include <dirent.h>
// #include <sys/stat.h>

// void dealDir(const char* path)
// {
//     DIR* dir;
//     struct dirent* entry;
//     struct stat * statbuf;

//     // opendir 返回 DIR*
//     dir = opendir(path);
//     // readdir 返回 struct dirent*
//     while ( (entry = readdir(dir)) != NULL)
//     {
//         // lstat 解析 每一个文件，然后 stat 是输出型参数
//         lstat(path,statbuf);
//     }

//     closedir(dir);
// }

// int main()
// {

//     char buf[100];
//     getcwd(buf,sizeof(buf));
// }


// lab 4  cp 
//  主要是文件的一堆操作 DealDir 和 DealFile 两种

// #include <fcntl.h>
// #include <stdio.h>

// void DealDir()
// {

// }

// void DealFile (char* destPath,char* srcPath)
// {
//     open(srcPath,O_CREAT|O_RDWR);
//     ssize_t bytes_read;

//     int fd;
//     while(bytes_read = read(fd,buf,sizeof(buf)))
//     {
//         if(write(fd,buf,bytes_read) != bytes_read)
//     }
// }

// int main(int argc,char** argv)
// {
//     // DealDir();
//     // DealFile();
// }


/// lab 5  fork 多进程
/// 

// #include <cstddef>
// #include <unistd.h>
// #include <sys/wait.h>

// void FatherDo()
// {

// }

// int main()
// {
//     if (1)
//     {
//         pid_t pid = fork();
//         if(pid == 0) // 子进程
//         {
//             // execl (path,name,args,NULL);
//             execl("./mycp","mycp",args,NULL);
//         }
//         else if (pid >0 ) // 父进程
//         {
//             int status;
//             pid_t ret = waitpid(pid,&status,0);
//         }
//     }
//     else 
//     {
//         FatherDo();
//     }
// }

// lab 7 
// cdev 字符文件操作核心

// static struct file_operations fp = {
//     .owner = THIS_MOUDLE,
//     .open = fp_open,
//     .read 
//     .write
//     .release
// }

// static int __int my_int(void) {
//     printk(KERN_ERR "Start Init\n");
// }


// module_init(my_init)
// module_init(my_exit)


// lab 2 

// 共享内存 1. 获取  2. 绑定  3. 解绑  4. 取消
// #include <cstddef>
// #include <sys/shm.h>
// #include <fcntl.h>
// #include <stdio.h>

// int main()
// {
//     int size;
//     int shmid = shmget(1234,size,0666 |IPC_CREAT);
    

//     void* addr= shmat(shmid,NULL,0);

//     shmdt(addr);
//     shmctl(shmid,IPC_RMID,NULL);

// }

// 线程的 信号量
// #include <semaphore.h>

// int main()
// {
//     sem_t sem1;
//     // 0 表示线程间共享  非0 
//     sem_init(&sem1,0,1);
//     // P 操作
//     sem_wait(&sem1);
//     // V 操作
//     sem_post(&sem1);
// }

// #include <sys/sem.h>

// int main()
// {
//     int semid = semget(1234,1,IPC_CREAT |0666);

// }

// #include <stdio.h>
// #include <semaphore.h>

// int main()
// {
//     sem_t sem;
//     sem_init(&sem,0,1);
//     // P V 操作
//     // P 操作申请资源  V 操作释放资源
//     sem_wait(&sem); // P
//     sem_post(&sem); // V 

//     sem_destroy(&sem);
// }

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define KEY_SEM 0x1234   // 信号量键值
#define KEY_SHM 0x5678   // 共享内存键值
#define NUM_PROCESSES 4  // 并发进程数

// 共享内存结构体（存储账户余额）
typedef struct {
    int balance;
} Account;

// 信号量操作联合体（System V 要求）
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// 初始化信号量（初始值 = 1，实现互斥锁）
int init_sem() {
    int sem_id = semget(KEY_SEM, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }
    
    union semun arg;
    arg.val = 1; // 初始化为1（二值信号量）
    if (semctl(sem_id, 0, SETVAL, arg) == -1) {
        perror("semctl init failed");
        exit(EXIT_FAILURE);
    }
    return sem_id;
}

// P操作（等待信号量，减1）
void sem_wait(int sem_id) {
    struct sembuf op = {0, -1, SEM_UNDO}; // SEM_UNDO 防止进程崩溃死锁
    if (semop(sem_id, &op, 1) == -1) {
        perror("semop wait failed");
        exit(EXIT_FAILURE);
    }
}

// V操作（释放信号量，加1）
void sem_signal(int sem_id) {
    struct sembuf op = {0, 1, SEM_UNDO};
    if (semop(sem_id, &op, 1) == -1) {
        perror("semop signal failed");
        exit(EXIT_FAILURE);
    }
}

// 子进程函数：模拟存款操作
void child_process(int sem_id, Account *acc) {
    for (int i = 0; i < 3; i++) {
        sem_wait(sem_id); // 🔒 进入临界区前获取锁
        
        // === 临界区开始 ===
        int old_balance = acc->balance;
        sleep(1); // 模拟操作延迟（放大竞争条件）
        acc->balance = old_balance + 100; // 存入100元
        printf("进程 %d: 存款 +100 | 余额: %d\n", getpid(), acc->balance);
        // === 临界区结束 ===
        
        sem_signal(sem_id); // 🔓 释放锁
    }
    exit(0);
}

int main() {
    // 1. 初始化信号量（互斥锁）
    int sem_id = init_sem();
    
    // 2. 创建共享内存（存储账户余额）
    int shm_id = shmget(KEY_SHM, sizeof(Account), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    Account *acc = (Account*)shmat(shm_id, NULL, 0);
    acc->balance = 1000; // 初始余额1000元

    // 3. 创建多个子进程并发操作账户
    for (int i = 0; i < NUM_PROCESSES; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            child_process(sem_id, acc); // 子进程执行存款操作
        } else if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
    }

    // 4. 等待所有子进程结束
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    // 5. 输出最终余额（应为 1000 + 100 * 4 * 3 = 2200）
    printf("\n最终余额: %d (理论值: 2200)\n", acc->balance);

    // 6. 清理资源
    shmdt(acc);                          // 分离共享内存
    shmctl(shm_id, IPC_RMID, NULL);       // 删除共享内存
    semctl(sem_id, 0, IPC_RMID);          // 删除信号量
    return 0;
}