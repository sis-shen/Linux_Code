#pragma once
#include <iostream>
#include <vector>
#include <pthread.h>
#include <string>
#include <queue>
#include <unistd.h>
struct ThreadInfo//储存线程信息
{
    pthread_t tid;
    std::string name;
};

static const int defaultnum = 5;//默认5个线程

template<class Task>
class ThreadPool
{
public:
    void Lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    void Unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    void Wakeup()
    {
        pthread_cond_signal(&_cond);
    }

    void ThreadSleep()
    {
        pthread_cond_wait(&_cond,&_mutex);
    }

    bool IsQueueEmpty()
    {
        return _tasks.empty();//封装一下接口
    }

    std::string GetThreadName(pthread_t tid)
    {
        for(const auto &ti:_threads)
        {
            if(tid == ti.tid) return ti.name;
        }
        return "None";
    }

    Task Pop()
    {
        //确认只在加锁的情况下调用
        Task t = _tasks.front();
        _tasks.pop();
        return t;
    }

public:
    ThreadPool(int num = defaultnum):_threads(num)
    {
        pthread_mutex_init(&_mutex,nullptr);
        pthread_cond_init(&_cond,nullptr);
    }

    ~ThreadPool()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

    static void *HandlerTask(void* args)
    {
        ThreadPool<Task> *tp = static_cast<ThreadPool<Task> *>(args);
        std::string name = tp->GetThreadName(pthread_self());

        // std::cout<<"get this success"<<std::endl;//DEBUG

        while(true)
        {
            tp->Lock();
            while(tp->IsQueueEmpty())//访问临界资源
            {
                tp->ThreadSleep();
            }
            // std::cout<<"task ready"<<std::endl; //DEBUG
            Task t = tp->Pop();
            // std::cout<<"Pop success"<<std::endl; //DEBUG
            tp->Unlock();

            t();//处理线程私有的任务，不需要加锁,可以并发
            std::cout<<name<<" run :result :" << t._result<<std::endl;
            sleep(4);
        }

    }
    void Start()
    {
        int num = _threads.size();
        for(int i = 0;i<num;++i)
        {
            _threads[i].name = "thread-"+std::to_string(i);
            pthread_create(&(_threads[i].tid),nullptr,HandlerTask,this);
            std::cout<<_threads[i].name<<" create successfully"<<std::endl;
        }
    }

    void Push(const Task& t)
    {
        Lock();
        _tasks.push(t);//推送任务
        Wakeup();
        Unlock();
    }

private:
    std::vector<ThreadInfo> _threads;//线程池
    std::queue<Task> _tasks;//任务队列

    pthread_mutex_t _mutex;//锁
    pthread_cond_t _cond;//条件队列
};