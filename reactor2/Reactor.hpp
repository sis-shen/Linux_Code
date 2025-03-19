#pragma once

#include <memory.h>
#include <memory>
#include "ThreadPool.hpp"
class ReactorServer
{
public:
    ReactorServer(uint16_t port,std::shared_ptr<EventHandler>handler = std::make_shared<EchoHandler>())
    :_port(port),_selector(),_pool(handler)
    {
        _listen_sock = socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0);
        _pool.setSelector(&_selector);
    }

    void start()
    {
        Bind();
        Listen();
        _selector.Add(_listen_sock);

        for(;;)
        {
            std::vector<struct epoll_event>events;
            _selector.Wait(&events);//获取就绪事件
            Dispatcher(events);//处理事件
        }
    }

    ~ReactorServer()
    {
        close(_listen_sock);
    }

protected:
    void Dispatcher(const std::vector<struct epoll_event>&events)
    {
        printf("get events: %lu\n",events.size());
        for(const auto& event:events)
        {
            if(event.data.fd == _listen_sock)
            {
                Acceptor();
            }
            else
            {
                //读写分离
                if(event.events & EPOLLIN)
                {
                    //创建写事件任务
                    _pool.addTask(event.data.fd,reactor::Task::READ);
                }
                if(event.events & EPOLLOUT)
                {
                    _pool.addTask(event.data.fd,reactor::Task::SEND);
                }
            }
        }
    }

    void Acceptor()
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int newfd = accept(_listen_sock,(struct sockaddr*)&peer,&len);
        if(newfd < 0)
        {
            printf("[Fatal]accept error,errno: %d error string:%s\n",errno,strerror(errno));
            exit(-1);
        }
        //不关心客户端信息
        // char ipstr[64] = {0};
        // inet_ntop(AF_INET,&peer.sin_addr,ipstr,sizeof(ipstr));
        // 设置非阻塞标志
        // int flags = fcntl(newfd, F_GETFL, 0);
        // fcntl(newfd, F_SETFL, flags | O_NONBLOCK);//设置为非阻塞
        _selector.Add(newfd);
    }

    void Bind()
    {
        int opt = 1;
        setsockopt(_listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;//自动分配IP
        if(bind(_listen_sock,(const struct sockaddr*)&local,sizeof(local)) < 0)
        {
            printf("[Fatal]bind error,errno: %d error string:%s\n",errno,strerror(errno));
            exit(-1);
        }
    }

    void Listen(int backlog = 10)
    {
        if(listen(_listen_sock,backlog) < 0)
        {
            printf("[Fatal]listen error,errno: %d error string:%s\n",errno,strerror(errno));
            exit(-1);
        }
    }

protected:
    int _listen_sock;
    uint16_t _port;
    Selector _selector;
    reactor::ThreadPool _pool;
};