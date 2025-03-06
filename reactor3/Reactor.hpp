#pragma once

#include <memory.h>
#include <memory>
#include "ThreadPool.hpp"
class ReactorServer
{
public:
    ReactorServer(uint16_t port,int sub_count = 5,std::shared_ptr<EventHandler>handler = std::make_shared<EchoHandler>())
    :_port(port),_selector(),_pool(handler),_sub_count(sub_count),_sub_selectors(sub_count)
    {
        _listen_sock = socket(AF_INET,SOCK_STREAM | SOCK_NONBLOCK,0);
    }

    void start()
    {
        //启动从Reactor线程
        for(int i = 0;i<_sub_count;++i)
        {
            printf("从Reactor: %d 启动\n",i);
            _sub_reactors.emplace_back(std::bind(&ReactorServer::subReactorLoop,this,i));
        }
        printf("从Reactor启动成功！\n");

        Bind();
        Listen();
        _selector.Add(_listen_sock);

        printf("开始监听连接\n");
        // int cnt = 0;
        for(;;)
        {
            // cnt++;
            // printf("loop: %d\n",cnt);
            std::vector<struct epoll_event>events;
            _selector.Wait(&events);//获取就绪事件
            if(!events.empty())
                Acceptor();
        }
    }

    ~ReactorServer()
    {
        close(_listen_sock);
        for(auto& t:_sub_reactors)
        {
            t.join();
        }
    }

    void subReactorLoop(int sub_num)
    {
        Selector& _sub_selector = _sub_selectors[sub_num];

        for(;;)
        {
            std::vector<struct epoll_event>events;
            {
                _sub_selector.Wait(&events);
                Dispatcher(events,&_sub_selector);
            }
        }
    }

protected:
    void Dispatcher(const std::vector<struct epoll_event>&events,Selector* selector)
    {
        // printf("get events: %lu\n",events.size());
        for(const auto& event:events)
        {
            //读写分离
            if(event.events & EPOLLIN)
            {
                //创建读事件任务
                _pool.addTask(event.data.fd,reactor::Task::READ,selector);
            }
            if(event.events & EPOLLOUT)
            {
                _pool.addTask(event.data.fd,reactor::Task::SEND,selector);
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
        int pos = getSubPos(newfd);
        printf("[sub_selector:%d]",pos);
        _sub_selectors[pos].Add(newfd);
    }

    int getSubPos(int fd)
    {
        //简单整个哈希函数
        return fd%_sub_count;
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
    std::vector<std::thread> _sub_reactors;
    std::vector<Selector> _sub_selectors;
    int _sub_count;
    reactor::ThreadPool _pool;
};