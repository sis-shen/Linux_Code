#pragma once

#include <iostream>
#include <sys/epoll.h>
#include "Socket.hpp"
#include "Epoller.hpp"
#include <memory>
#include <string>

static const uint16_t defualtport = 25565;

const uint32_t EVENT_IN = EPOLLIN;
const uint32_t EVENT_OUT = EPOLLOUT;

class EpollServer
{
    static const int num = 64;

public:
    EpollServer(uint16_t port = defualtport) : _port(port), _listensocket_ptr(new Sock()), _epoller_ptr(new Epoller())
    {
    }

    ~EpollServer() { _listensocket_ptr->Close(); }

    void Init()
    {
        _listensocket_ptr->Socket();
        _listensocket_ptr->Bind(_port);
        _listensocket_ptr->Listen();
    }

    void HandlerEvent(struct epoll_event revs[], int n)
    {
        for (int i = 0; i < n; ++i)
        {
            uint32_t events = revs[i].events;
            int fd = revs[i].data.fd;
            if (events & EVENT_IN)
            {
                if (fd == _listensocket_ptr->Fd())
                {
                    // 获取新链接
                    Acceptor();
                }
                else
                {
                    Recver(fd);
                }
            }
            else if (events & EVENT_OUT)
            {
                //TODO
            }
            else
            {
                //TODO
            }
        }
    }

    void Recver(int fd)
    {
        char buffer[1024] = {0};
        int n = read(fd, buffer, sizeof(buffer));
        if (n > 0)
        {
            std::cout << "client msg[#] " << buffer << std::endl;
        }
        else if (n == 0)
        {
            log(Info, "fd:%d closed the link", fd);
            _epoller_ptr->EpollerUpdate(EPOLL_CTL_DEL, fd, EVENT_IN);
            close(fd); // 必须后面删除
        }
        else
        {
            log(Error, "fd:%d read error", fd);
            _epoller_ptr->EpollerUpdate(EPOLL_CTL_DEL, fd, EVENT_IN);
            close(fd); // 必须后面删除
        }
    }

    void Acceptor()
    {
        std::string clientip;
        uint16_t clientport;
        int sock = _listensocket_ptr->Accept(&clientip, &clientport);
        if (sock < 0)
        {
            log(Error, "get new link failed");
            return;
        }
        _epoller_ptr->EpollerUpdate(EPOLL_CTL_ADD, sock, EVENT_IN);
    }

    void Start()
    {
        _epoller_ptr->EpollerUpdate(EPOLL_CTL_ADD, _listensocket_ptr->Fd(), EVENT_IN);
        struct epoll_event revs[num];

        for (;;)
        {
            int n = _epoller_ptr->EpollerWait(revs, num);
            if (n > 0)
            {
                // log(Debug,"event happend fd:%d",revs[0].data.fd);
                HandlerEvent(revs, n);
            }
            else if (n == 0)
            {
                log(Info, "time out...");
            }
            else
            {
                log(Error, "epoll_wait error");
            }
        }
    }

private:
    std::shared_ptr<Sock> _listensocket_ptr;
    std::shared_ptr<Epoller> _epoller_ptr;
    uint16_t _port;
};