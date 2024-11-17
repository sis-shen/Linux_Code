// #include <unistd.h>
// #include <stdlib.h>
// #include <stdio.h>

// #include <string.h>
// #include <pthread.h>
// void *rout(void *arg)
// {
//     int i;
//     for (;;)
//     {
//         printf("[%lu]I'am thread 1,got arg:%d\n",pthread_self(),*(int*)arg);
//         sleep(1);
//     }
// }
// int main(void)
// {
//     pthread_t tid;//储存tid的变量
//     int ret;
//     int arg = 114514;//准备参数
//     //创建新线程
//     if ((ret = pthread_create(&tid, NULL, rout, &arg)) != 0)
//     {
//         //创建失败
//         fprintf(stderr, "pthread_create : %s\n", strerror(ret));
//         exit(EXIT_FAILURE);
//     }
//     int i;
//     for (;;)
//     {
//         //主线程输出
//         printf("I'am main thread,tid:%lu\n",tid);
//         sleep(1);
//     }
// }

// 操作共享变量会有问题的售票系统代码
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>

// int ticket = 100;//抢票程序，预留100张票
// pthread_mutex_t mutex;//准备一个互斥量

// void *route(void *arg)
// {
//     char *id = (char *)arg;//从参数里获取线程名
//     while (1)
//     {
//         pthread_mutex_lock(&mutex);//进入if判断前就得加锁，因为判断时ticket也是共享的
//         if (ticket > 0)
//         {
//             printf("%s sells ticket:%d\n", id, ticket);
//             ticket--;
//             pthread_mutex_unlock(&mutex);//完成对共享资源的访问，可以释放互斥量
//             //usleep不访问共享资源，放在锁外面，以提高多线程并行效率
//             usleep(1000);//降低每次抢票的速度
//         }
//         else
//         {
//             pthread_mutex_unlock(&mutex);//原理同上
//             break;
//         }
//     }
//     return nullptr;
// }
// int main(void)
// {
//     pthread_mutex_init(&mutex,nullptr);//初始化互斥量
//     pthread_t t1, t2, t3, t4;//准备四个线程id
//     char name1[] = "thread 1";//准备四个线程名
//     char name2[] = "thread 2";
//     char name3[] = "thread 3";
//     char name4[] = "thread 4";

//     pthread_create(&t1, NULL, route, name1);//创建四个线程
//     pthread_create(&t2, NULL, route, name2);
//     pthread_create(&t3, NULL, route, name3);
//     pthread_create(&t4, NULL, route, name4);
//     pthread_join(t1, NULL);//进程等待
//     pthread_join(t2, NULL);
//     pthread_join(t3, NULL);
//     pthread_join(t4, NULL);

//     pthread_mutex_destroy(&mutex);//销毁互斥量

//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>

// pthread_cond_t cond;   // 创建信号量
// pthread_mutex_t mutex; // 创建互斥量

// void *chicken(void *arg)
// {
//     while (1)
//     {
//         pthread_cond_wait(&cond, &mutex);
//         printf("鸡哥获得了一个篮球\n");
//     }
// }

// void *kunkun(void *arg)
// {
//     for (int i = 0; i < 5; ++i)
//     {
//         printf("坤坤扔出了一个篮球[%d]\n", i);
//         pthread_cond_signal(&cond);
//         sleep(1);
//     }
// }

// int main()
// {
//     pthread_t t1, t2;
//     pthread_cond_init(&cond, NULL);
//     pthread_mutex_init(&mutex, NULL);
//     pthread_create(&t1, NULL, chicken, NULL);
//     pthread_create(&t2, NULL, kunkun, NULL);
//     pthread_join(t1, NULL);
//     pthread_join(t2, NULL);
//     pthread_mutex_destroy(&mutex);
//     pthread_cond_destroy(&cond);

//     return 0;
// }

// #include <iostream>
// #include <queue>
// #include <stdlib.h>
// #include <pthread.h>

// #define NUM 8 //定义阻塞队列的默认容量

// class BlockQueue
// {
// private:
//     std::queue<int> q;
//     int cap;
//     pthread_mutex_t lock;
//     pthread_cond_t full;
//     pthread_cond_t empty;

// private:
//     void LockQueue()
//     {
//         pthread_mutex_lock(&lock);//给队列上锁
//     }
//     void UnLockQueue()
//     {
//         pthread_mutex_unlock(&lock);//给队列解锁
//     }
//     void ProductWait()
//     {
//         pthread_cond_wait(&full, &lock);//生产者等待信号量
//     }
//     void ConsumeWait()
//     {
//         pthread_cond_wait(&empty, &lock);//消费者等待信号量
//     }
//     void NotifyProduct()
//     {
//         pthread_cond_signal(&full);//唤醒生产者
//     }
//     void NotifyConsume()
//     {
//         pthread_cond_signal(&empty);//唤醒消费者
//     }
//     bool IsEmpty()
//     {
//         return (q.size() == 0 ? true : false);
//     }
//     bool IsFull()
//     {
//         return (q.size() == cap ? true : false);
//     }

// public:
//     BlockQueue(int _cap = NUM) : cap(_cap)//构造函数
//     {
//         pthread_mutex_init(&lock, NULL);
//         pthread_cond_init(&full, NULL);
//         pthread_cond_init(&empty, NULL);
//     }
//     void PushData(const int &data)
//     {
//         LockQueue();
//         while (IsFull())
//         {
//             NotifyConsume();//队列已满，唤醒消费者
//             std::cout << "queue full, notify consume data, product stop." << std::endl;
//             ProductWait();//生产者等待条件变量
//         }
//         q.push(data);
//         // NotifyConsume();
//         UnLockQueue();//释放消息队列的锁
//     }
//     void PopData(int &data)
//     {
//         LockQueue();
//         while (IsEmpty())
//         {
//             NotifyProduct();//队列已空，唤醒生产者
//             std::cout << "queue empty, notify product data, consume stop." << std::endl;
//             ConsumeWait();//等待生产者
//         }
//         data = q.front();
//         q.pop();
//         // NotifyProduct();
//         UnLockQueue();
//     }
//     ~BlockQueue()
//     {
//         pthread_mutex_destroy(&lock);
//         pthread_cond_destroy(&full);
//         pthread_cond_destroy(&empty);
//     }
// };
// void *consumer(void *arg)
// {
//     BlockQueue *bqp = (BlockQueue *)arg;//传入对象的this指针
//     int data;
//     for (;;)
//     {
//         bqp->PopData(data);
//         std::cout << "Consume data done : " << data << std::endl;
//     }
// }
// // more faster
// void *producter(void *arg)
// {
//     BlockQueue *bqp = (BlockQueue *)arg;
//     srand((unsigned long)time(NULL));
//     for (;;)
//     {
//         int data = rand() % 1024;
//         bqp->PushData(data);
//         std::cout << "Prodoct data done: " << data << std::endl;
//         // sleep(1);
//     }
// }
// int main()
// {
//     BlockQueue bq;
//     pthread_t c, p;
//     pthread_create(&c, NULL, consumer, (void *)&bq);
//     pthread_create(&p, NULL, producter, (void *)&bq);
//     pthread_join(c, NULL);
//     pthread_join(p, NULL);
//     return 0;
// }

// 懒汉模式, 线程安全
template <typename T>
class Singleton
{
    volatile static T *inst; // 需要设置 volatile 关键字, 否则可能被编译器优化.
    static std::mutex lock;

public:
    static T *GetInstance()
    {
        if (inst == NULL)
        {                // 双重判定空指针, 降低锁冲突的概率, 提高性能.
            lock.lock(); // 使用互斥锁, 保证多线程情况下也只调用一次 new.
            if (inst == NULL)
            {
                inst = new T();
            }
            lock.unlock();
        }
        return inst;
    }
};
