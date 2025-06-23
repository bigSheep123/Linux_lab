#include <cassert>
#include <cstddef>
#include<pthread.h>
#include <iostream>
#include <semaphore.h>
#include <unistd.h>
#include <string>

// 哲学家用餐问题，可以用 2种思路去实现  1. mutex  2. sem 
// 我这里选择用 mutex 的方式去实现
// 我这里使用 奇偶 哲学家解决了这个问题

#define NUM 5

typedef struct Philosopher 
{
    pthread_t tid;
    int  id;
    std::string name;
} Philosopher;

// 筷子 是 临界资源，给每一个筷子一把锁以及 id 号
typedef struct Chopstick
{
    pthread_mutex_t lock;
    int id;
} Chopstick;

Chopstick Chopsticks[NUM];
Philosopher Philosophers[NUM];

void delay()
{
    srand((unsigned int)time(nullptr));
    unsigned int time = rand() % 4000 + 2100;
    usleep(time);
}

void think()  {   delay();  }
void eat() { sleep(1); }

// 如果所有哲学家同时拿起一边的筷子会导致死锁问题
void* Action(void* args)
{
    int  num = *(int*)args;
    while (1)
    {
        think();
        sleep(3);
        pthread_mutex_lock(&Chopsticks[num].lock);
        printf("%d 号哲学家 %s 拿起左边的 %d 号筷子成功\n",
               num, Philosophers[num].name.c_str(), num);

        pthread_mutex_lock(&Chopsticks[(num + 1) % NUM].lock);
        printf("%d 号哲学家 %s 拿起右边的 %d 号筷子成功\n",
               num, Philosophers[num].name.c_str(), (num + 1) % NUM);

        printf("=== %d 号哲学家 %s 开始吃饭了 ===\n", num,
               Philosophers[num].name.c_str());
        eat();
        printf("依次放下左右两边的筷子\n");
        pthread_mutex_unlock(&Chopsticks[num].lock);
        pthread_mutex_unlock(&Chopsticks[(num + 1) % NUM].lock);
    }

    return nullptr;
}

// 规定奇数哲学家用左边的筷子，偶数哲学家用右边的筷子
// 奇偶性问题能够很好的分配资源，消除竞争
void* SovleAction(void* args)
{
    int  num = *(int*)args;
    while (1)
    {
        think();
        sleep(3);
        if(num % 2 == 0)
        {
            pthread_mutex_lock(&Chopsticks[num].lock);
            printf("%d 号哲学家 %s 拿起左边的 %d 号筷子成功\n",
                   num, Philosophers[num].name.c_str(), num);

            pthread_mutex_lock(&Chopsticks[(num + 1) % NUM].lock);
            printf("%d 号哲学家 %s 拿起右边的 %d 号筷子成功\n",
                   num, Philosophers[num].name.c_str(), (num + 1) % NUM);
        }
        else if (num % 2 == 1)
        {
            pthread_mutex_lock(&Chopsticks[(num + 1) % NUM].lock);
            printf("%d 号哲学家 %s 拿起右边的 %d 号筷子成功\n",
                   num, Philosophers[num].name.c_str(), (num + 1) % NUM);

            pthread_mutex_lock(&Chopsticks[num].lock);
            printf("%d 号哲学家 %s 拿起左边的 %d 号筷子成功\n",
                   num, Philosophers[num].name.c_str(), num);
        }

        printf("=== %d 号哲学家 %s 开始吃饭了 ===\n", num,
               Philosophers[num].name.c_str());
        eat();
        printf("依次放下左右两边的筷子\n");
        pthread_mutex_unlock(&Chopsticks[num].lock);
        pthread_mutex_unlock(&Chopsticks[(num + 1) % NUM].lock);
    }

    return nullptr;
}

void Name()
{
    Philosophers[0].name = "孔子";
    Philosophers[1].name = "老子";
    Philosophers[2].name = "孟子";
    Philosophers[3].name = "刁寒";
    Philosophers[4].name = "牛顿";
}

int main()
{
    for(int i = 0; i < NUM ; i++)
    {
        Chopsticks[i].id = i;
        pthread_mutex_init(&Chopsticks[i].lock, nullptr);        
    }
    Name();
    for(int i = 0; i < NUM ; i++)
    {
        Philosophers[i].id = i;

        // Action --> 死锁
        //
        pthread_create(&Philosophers[i].tid,
                      nullptr,SovleAction,&Philosophers[i].id);
    }

    for(int i = 0; i < NUM ; i ++)
    {
        pthread_join(Philosophers[i].tid,nullptr);
    }

    for(int i = 0; i < NUM ; i++)
    {
        pthread_mutex_destroy(&Chopsticks[i].lock);
    }

    return 0;
}