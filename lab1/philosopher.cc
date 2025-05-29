#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define PHILOSOPHER_COUNT 5

pthread_mutex_t chopsticks[PHILOSOPHER_COUNT];
int philosopher_ids[PHILOSOPHER_COUNT] = {0, 1, 2, 3, 4};

// 创造延迟
// 1-6ms随机延迟
void random_delay() {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = (rand() % 5000000) + 1000000;  
    nanosleep(&ts, NULL);
}

void initialize_mutexes() {
    for (int i = 0; i < PHILOSOPHER_COUNT; i++)
        pthread_mutex_init(&chopsticks[i], NULL);
}

//奇数编号哲学家优先获取右侧筷子，偶数编号哲学家优先获取左侧筷子
//打破循环等待条件：通过不同顺序获取资源，确保不会形成环形等待链
void alternate_order_eat(int philosopher_id) {
    if (philosopher_id % 2 == 0) {
        pthread_mutex_lock(&chopsticks[philosopher_id]);
        pthread_mutex_lock(&chopsticks[(philosopher_id + 1) % PHILOSOPHER_COUNT]);
    } else {
        pthread_mutex_lock(&chopsticks[(philosopher_id + 1) % PHILOSOPHER_COUNT]);
        pthread_mutex_lock(&chopsticks[philosopher_id]);
    }
    
    printf("Philosopher %d is eating\n", philosopher_id);
    random_delay();
    
    pthread_mutex_unlock(&chopsticks[philosopher_id]);
    pthread_mutex_unlock(&chopsticks[(philosopher_id + 1) % PHILOSOPHER_COUNT]);
}

void* philosopher_behavior(void* arg) {
    int id = *(int*)arg;
    while (1) {
        printf("Philosopher %d is thinking\n", id);
        random_delay();
        printf("Philosopher %d is hungry\n", id);
        alternate_order_eat(id);
    }
    return NULL;
}

int main() {
    pthread_t philosophers[PHILOSOPHER_COUNT];
    srand(time(NULL));
    initialize_mutexes();

    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        int result = pthread_create(&philosophers[i], NULL, philosopher_behavior, &philosopher_ids[i]);
        if (result != 0) {
            fprintf(stderr, "Thread creation error: %s\n", strerror(result));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        pthread_join(philosophers[i], NULL);
        pthread_mutex_destroy(&chopsticks[i]);
    }
    return 0;
}