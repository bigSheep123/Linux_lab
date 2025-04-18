#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_rwlock_t rwlock;

void* reader(void* arg) {
    pthread_rwlock_rdlock(&rwlock);    // 第一次加读锁
    printf("Reader %ld: 第一次读锁成功\n", (long)arg);

    // 模拟在未释放读锁时再次尝试加读锁
    pthread_rwlock_rdlock(&rwlock);    // 第二次加读锁（可能阻塞）
    printf("Reader %ld: 第二次读锁成功\n", (long)arg);

    pthread_rwlock_unlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void* writer(void* arg) {
    pthread_rwlock_wrlock(&rwlock);    // 尝试加写锁（阻塞直到读锁释放）
    pthread_rwlock_wrlock(&rwlock);    // 尝试加写锁（阻塞直到读锁释放）
    pthread_rwlock_wrlock(&rwlock);    // 尝试加写锁（阻塞直到读锁释放）
    pthread_rwlock_wrlock(&rwlock);    // 尝试加写锁（阻塞直到读锁释放）
    pthread_rwlock_wrlock(&rwlock);    // 尝试加写锁（阻塞直到读锁释放）
    printf("Writer: 写锁成功\n");
    pthread_rwlock_unlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

int main() {
    pthread_rwlock_init(&rwlock, NULL);
    pthread_t t1, t2;

    pthread_create(&t1, NULL, reader, (void*)1);
    pthread_create(&t2, NULL, writer, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_rwlock_destroy(&rwlock);
    return 0;
}