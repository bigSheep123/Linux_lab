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

// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>

// void *thr_fn1(void *arg)
// {
// 	printf("thread 1 returning\n");
// 	return (void *)1;
// }

// void *thr_fn2(void *arg)
// {
// 	printf("thread 2 exiting\n");
// 	pthread_exit((void *)2);
// 	exit(20);
// 	// return  20;
// }

// void *thr_fn3(void *arg)
// {
// 	while(1) {
// 		printf("thread 3 writing\n");
// 		sleep(1);
// 	}
// }

// int main(void)
// {
// 	pthread_t   tid;
// 	void        *tret;

// 	pthread_create(&tid, NULL, thr_fn1, NULL);
// 	pthread_join(tid, &tret);
// 	printf("thread 1 exit code %d\n", (int)tret);

// 	pthread_create(&tid, NULL, thr_fn2, NULL);
// 	pthread_join(tid, &tret);
// 	printf("thread 2 exit code %d\n", (int)tret);

// 	pthread_create(&tid, NULL, thr_fn3, NULL);
// 	sleep(3);
// 	pthread_cancel(tid);
// 	pthread_join(tid, &tret);
// 	printf("thread 3 exit code %d\n", (int)tret);

// 	//PTHREAD_CANCELED; // -1
// 	return 0;
// }

// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <unistd.h>

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
// 	printids(arg);
// 	return NULL;
// }

// int main(void)
// {
// 	int err;

// 	err = pthread_create(&ntid, NULL, thr_fn, ("new thread: "));
// 	if (err != 0) {
// 		fprintf(stderr, "can't create thread: %s\n", strerror(err));
// 		exit(1);
// 	}
// 	printids("main thread:");
// 	sleep(1);

// 	return 0;
// }

// mutex 多个线程同时访问共享数据时可能会冲突

// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>

// #define NLOOP 100000

// int counter;                /* incremented by threads */

// void *doit(void *);

// int main(int argc, char **argv)
// {
// 	pthread_t tidA, tidB;

// 	pthread_create(&tidA, NULL, &doit, NULL);
// 	pthread_create(&tidB, NULL, &doit, NULL);

//         /* wait for both threads to terminate */
// 	pthread_join(tidA, NULL);
// 	pthread_join(tidB, NULL);

// 	return 0;
// }

// void *doit(void *vptr)
// {
// 	int    i, val;

// 	/*
// 	 * Each thread fetches, prints, and increments the counter NLOOP times.
// 	 * The value of the counter should increase monotonically.
// 	 */

// 	for (i = 0; i < NLOOP; i++) {
// 		val = counter;
// 		printf("%x: %d\n", (unsigned int)pthread_self(), val + 1);
// 		counter = val + 1;
// 	}

// 	return NULL;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>


// #define NLOOP 5000

// int counter;                /* incremented by threads */
// pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

// void *doit(void *);

// int main(int argc, char **argv)
// {
// 	pthread_t tidA, tidB;

// 	pthread_create(&tidA, NULL, doit, NULL);
// 	pthread_create(&tidB, NULL, doit, NULL);

//         /* wait for both threads to terminate */
// 	pthread_join(tidA, NULL);
// 	pthread_join(tidB, NULL);

// 	return 0;
// }

// void *doit(void *vptr)
// {
// 	int     i, val;

// 	/*
// 	 * Each thread fetches, prints, and increments the counter NLOOP times.
// 	 * The value of the counter should increase monotonically.
// 	 */

// 	for (i = 0; i < NLOOP; i++) {
// 		pthread_mutex_lock(&counter_mutex);

// 		val = counter;
// 		printf("%x: %d\n", (unsigned int)pthread_self(), val + 1);
// 		counter = val + 1;

// 		pthread_mutex_unlock(&counter_mutex);
// 	}

// 	return NULL;
// }

// #include <stdlib.h>
// #include <pthread.h>
// #include <stdio.h>
// #include <unistd.h>

// struct msg {
// 	struct msg *next;
// 	int num;
// };

// struct msg *head;
// pthread_cond_t has_product = PTHREAD_COND_INITIALIZER;
// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// void *consumer(void *p)
// {
// 	struct msg *mp;

// 	for (;;) {
// 		pthread_mutex_lock(&lock);
// 		while (head == NULL)
// 			pthread_cond_wait(&has_product, &lock);
// 		mp = head;
// 		head = mp->next;
// 		pthread_mutex_unlock(&lock);
// 		printf("Consume %d\n", mp->num);
// 		free(mp);
// 		sleep(rand() % 5);
// 	}
// }

// void *producer(void *p)
// {
// 	struct msg *mp;
// 	for (;;) {
// 		mp = malloc(sizeof(struct msg));
// 		mp->num = rand() % 1000 + 1;
// 		printf("Produce %d\n", mp->num);
// 		pthread_mutex_lock(&lock);
// 		mp->next = head;
// 		head = mp;
// 		pthread_mutex_unlock(&lock);
// 		pthread_cond_signal(&has_product);
// 		sleep(rand() % 5);
// 	}
// }

// int main(int argc, char *argv[]) 
// {
// 	pthread_t pid, cid;  

// 	srand(time(NULL));
// 	pthread_create(&pid, NULL, producer, NULL);
// 	pthread_create(&cid, NULL, consumer, NULL);
// 	pthread_join(pid, NULL);
// 	pthread_join(cid, NULL);
// 	return 0;
// }



