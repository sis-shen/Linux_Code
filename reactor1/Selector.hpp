#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <iostream>

//poll实现Selector
class Selector
{
public:
    Selector(){
        _epoll_fd = epoll_create(10);
    }   

    ~Selector(){
        close(_epoll_fd);
    }

    bool Add(const int& sock)
    {
        int fd = sock;
        printf("[Epoll Add] fd = %d\n",fd);
        epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLIN;
        int ret = epoll_ctl(_epoll_fd,EPOLL_CTL_ADD,fd,&ev);
        if(ret < 0)
        {
            perror("epoll_ctl ADD");
            return false;
        }
        return true;
    }

    bool Del(int fd)
    {
        printf("[Epoll Del] fd = %d\n",fd);
        int ret = epoll_ctl(_epoll_fd,EPOLL_CTL_DEL,fd,NULL);
        if(ret < 0)
        {
            perror("epoll_ctl DEL");
            return false;
        }
        return true;
    }

    bool Wait(std::vector<int>*output)
    {
        output->clear();
        epoll_event events[1000] = {0};
        int nfds = epoll_wait(_epoll_fd,events,(sizeof(events) / sizeof(events[0])),-1);
        if(nfds < 0)
        {
            perror("epoll_wait");
            return false;
        }
        for(int i = 0;i<nfds;++i)
        {
            output->push_back(events[i].data.fd);
        }
        return true;
    }

private:
    int _epoll_fd;
};

