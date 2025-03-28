// 筷子时并发资源，具有排他性
// #include<string>
// #include <vector>
// #define NUM 5

// typedef struct 
// {

// } Chopstick;

// enum status
// {
//     havinglunch,
//     thinking
// };


// typedef struct 
// {
//     std::string name;
//     Chopstick* left;
//     Chopstick* right;
//     status mystatus;
// } Philosopher;

// int main()
// {
//     int count = NUM;
//     std::vector<Philosopher*> Philosophers(NUM);
//     std::vector<Chopstick*> Chopsticks(NUM);

//     for(int i =0; i < Philosophers.size(); i++)
//     {
//         Philosophers[i]->name = "Philosopher" + std::to_string(i);
//     }

// }


#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#define NUM 5
#define CYCLENUM  10

pthread_mutex_t chopsticks[NUM];
pthread_mutex_t output_mutex;

typedef struct 
{
    pthread_t threadid;
    int id;
}Philosopher;

void actions(int id,const char* action)
{
    pthread_mutex_lock(&output_mutex);
    printf("哲学家 %d %s\n",id,action);
    pthread_mutex_unlock(&output_mutex);
}

void pick_up(int id)
{
    int left = id;
    int right = (id +1) % NUM;

       // 顺序策略：前4位先左后右，最后一位先右后左
    if (id == NUM - 1) {
        pthread_mutex_lock(&chopsticks[right]);
        pthread_mutex_lock(&chopsticks[left]);
    } else {
        pthread_mutex_lock(&chopsticks[left]);
        pthread_mutex_lock(&chopsticks[right]);
    }
}

void put_down(int id) {
    int left = id;
    int right = (id + 1) % NUM;

    pthread_mutex_unlock(&chopsticks[left]);
    pthread_mutex_unlock(&chopsticks[right]);
}

void* philosopherActions(void* arg)
{
    int id = *(int*) arg;

    for(int i =0; i < CYCLENUM; i++)
    {
        actions(id,"开始思考");
        usleep(100000);

        actions(id,"尝试拿筷子");
        pick_up(id);

        actions(id,"开始吃饭");
        usleep(100000);

        put_down(id);
        actions(id,"放下筷子");
    }

    return NULL;
}

int main()
{
   Philosopher* philosophers[NUM];

    // 互斥锁的初始化
   pthread_mutex_init(&output_mutex,NULL);
   for(int i = 0; i < NUM ; i++)
   {
        pthread_mutex_init(&chopsticks[i], NULL);
   }


   for(int i = 0; i < NUM; i++)
   {
        philosophers[i]->id = i;
        pthread_t thid = philosophers[i]->threadid;
        if(pthread_create(&thid, NULL, philosopherActions,&philosophers[i]->id) !=0 )
        {
            perror("thread failed");
            exit(EXIT_FAILURE);
        }
   }

   for(int i = 0; i < NUM ; i++ )
   {
        pthread_join(philosophers[i]->threadid, NULL);
   }

   for(int i = 0; i < NUM; i++)
   {
        pthread_mutex_destroy(&chopsticks[i]);
   }

    pthread_mutex_destroy(&output_mutex);

    return 0;
}