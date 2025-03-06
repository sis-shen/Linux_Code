#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <mutex>
#include <atomic>
#include "EventHandler.hpp"
namespace reactor
{
struct Task
{
    enum Type
    {
        READ,
        SEND
    };
    int fd;
    Type type;
    Selector* selector;
};

class ThreadPool
{
public:

    ThreadPool(std::shared_ptr<EventHandler> handler,int capacity=20,int n = 5)
    :_isRunning(true),_handler(handler),_capacity(capacity)
    {
        for(int i = 0;i<n;++i)
        {
            _pool.emplace_back(std::thread(std::bind(&ThreadPool::worker,this)));
        }
    }

    void worker()
    {
        for(;;)
        {
            if(_isRunning == false)return;
            Task task;
            {
                std::unique_lock lock(_mtx);
                if(_isRunning == false && _tasks.empty())
                    return;

                _pop_cond.wait(lock,[&]{
                    return !_tasks.empty() || !_isRunning;
                });
                if(_tasks.empty())return;
                task = _tasks.front();
                _tasks.pop();
                _push_cond.notify_all();//通知生产者可以生产了
            }
            if(task.type == Task::READ)
            {
                _handler->handle_read(task.fd,task.selector);
            }
            else
            {
                _handler->handle_write(task.fd,task.selector);
            }
        }
        
    }


    void addTask(int fd,Task::Type type,Selector* selector)
    {
        if(_isRunning == false)return;
        std::unique_lock<std::mutex> lock(_mtx);
        _push_cond.wait(lock,[&]{
            return _tasks.size() < _capacity;
        });//防止任务堆积过多

        _tasks.push({fd,type,selector});
        _pop_cond.notify_all();
    }

    ~ThreadPool()
    {
        _isRunning = false;//关闭运行
        _push_cond.notify_all();//唤醒所有进程
        _pop_cond.notify_all();
        for(int i = 0;i<_pool.size();++i)
        {
            _pool[i].join();
        }
    }

private:
    std::vector<std::thread> _pool;
    std::shared_ptr<EventHandler> _handler;
    int _capacity;
    std::queue<Task> _tasks;
    std::mutex _mtx;
    std::condition_variable _push_cond;
    std::condition_variable _pop_cond;
    std::atomic<bool> _isRunning;
};
}