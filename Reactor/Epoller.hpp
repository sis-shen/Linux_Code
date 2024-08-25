#pragma once

#include <sys/epoll.h>
#include <cerrno>
#include <string.h>
#include "log.hpp"
#include "nocopy.hpp"

extern Log log;

class Epoller:public nocopy
{
    static const int size = 128;
public:
    Epoller()
    {
        _epfd = epoll_create(size);
        if(_epfd == -1)
        {
            log(Fatal,"Epoll create failed, error:%s",strerror(errno));
        }
        else 
        {
            log(Info,"eppll_create succecced");
        }
    }
    ~Epoller()
    {
        if(_epfd>=0)close(_epfd);
    }

    int EpollerWait(struct epoll_event* revs,int num)
    {
        int n = epoll_wait(_epfd,revs,num,_timeout);

        return n;
    }

    int EpollerUpdate(int oper,int sock,uint32_t event)
    {
        int n = 0;
        if(oper == EPOLL_CTL_DEL)
        {
            n = epoll_ctl(_epfd,oper,sock,nullptr);
        }
        else
        {
            struct epoll_event ev;
            ev.events = event;
            ev.data.fd = sock;

            n = epoll_ctl(_epfd,oper,sock,&ev);
            if(n!=0)
            {
                log(Error,"epoll_ctl error");
            }
        }
    }
private:
    int _epfd;
    int _timeout = 3000;
};