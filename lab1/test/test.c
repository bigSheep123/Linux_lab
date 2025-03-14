// thread_t 类型是一个地址值， 属于同一个进程的多个线程调用 getpid可以得到相同的进程号
// pthread_self 得到的线程号各不相同

// pthread_t ntid;

// void printids(const char *s)
// {
// 	pid_t      pid;
// 	pthread_t  tid;

// 	pid = getpid();
// 	tid = pthread_self();
// 	printf("%s pid %u tid %u (0x%x)\n", s, (unsigned int)pid,
// 	       (unsigned int)tid, (unsigned int)tid);
// }

// void *thr_fn(void *arg)
// {
// 	printf("I am a new thread\n");
// 	sleep(1);
// 	printids(arg);
// 	return NULL;
// }

//     //    int pthread_create(pthread_t *restrict thread,
//     //                       const pthread_attr_t *restrict attr,
//     //                       void *(*start_routine)(void *),
//     //                       void *restrict arg);

// // pthread_attr_t

// int main(void)
// {
// 	int err;

// 	printf("%lu\n",ntid);
// 	err = pthread_create(&ntid, NULL, 
// 						thr_fn, "new thread: ");
// 	printf("%lu\n",ntid);
// 	if (err != 0) {
// 		fprintf(stderr, "can't create thread: %s\n", strerror(err));
// 		exit(1);
// 	}
// 	printids("main thread:");
// 	// sleep(1);

// 	return 0;
// }

// 只需要终止某个线程而不终止整个进程

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thr_fn1(void *arg)
{
	printf("thread 1 returning\n");
	return (void *)1;
}

void *thr_fn2(void *arg)
{
	printf("thread 2 exiting\n");
	pthread_exit((void *)2);
	exit(20);
	// return  20;
}

void *thr_fn3(void *arg)
{
	while(1) {
		printf("thread 3 writing\n");
		sleep(1);
	}
}

int main(void)
{
	pthread_t   tid;
	void        *tret;

	pthread_create(&tid, NULL, thr_fn1, NULL);
	pthread_join(tid, &tret);
	printf("thread 1 exit code %d\n", (int)tret);

	pthread_create(&tid, NULL, thr_fn2, NULL);
	pthread_join(tid, &tret);
	printf("thread 2 exit code %d\n", (int)tret);

	pthread_create(&tid, NULL, thr_fn3, NULL);
	sleep(3);
	pthread_cancel(tid);
	pthread_join(tid, &tret);
	printf("thread 3 exit code %d\n", (int)tret);

	//PTHREAD_CANCELED; // -1
	return 0;
}