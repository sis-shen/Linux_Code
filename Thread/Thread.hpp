#pragma once

/////////封装线程///////////

#include <iostream>
#include <string>
#include <ctime>
#include <pthread.h>
#include <functional>

typedef void (*callback_t)();

static int cnt = 1;//全局计数器

class Thread
{
private:
    static void *Routine(void* args)
    {
        Thread *thread = static_cast<Thread*>(args);
        thread->Entry();
    }

public:
    Thread(callback_t cb,):_tid(0),_name(""),_start_tmiestamp(0),_isrunning(false),_cb(cb)
    {}
    void Run()
    {
        _name = "thread-"+std::to_string(cnt++);
        _start_tmiestamp = time(nullptr);
        _isrunning = true;
        pthread_create(&_tid,nullptr,Routine,this);
    }
    void Join()
    {
        pthread_join(_tid,nullptr);
        _isrunning = false;
    }
    std::string Name()
    {
        return _name;
    }
    uint64_t StartTmieStamp()
    {
        return _start_tmiestamp;
    }
    bool IsRunning()
    {
        return _isrunning;
    }

    void Entry()
    {
        _cb();
    }
    ~Thread()
    {
        if(IsRunning())
        {
            pthread_join(_tid,nullptr);
        }
    }

private:
    pthread_t _tid;
    std::string _name;
    uint64_t _start_tmiestamp;
    bool _isrunning;

    callback_t _cb;
};