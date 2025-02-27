#include "EventHalder.hpp"
#include <memory.h>
#include <memory>
class ReactorServer
{
public:
    ReactorServer(uint16_t port,std::shared_ptr<EventHandler>handler = std::make_shared<EchoHandler>()):_port(port),_handler(handler)
    {
        _listen_sock = socket(AF_INET,SOCK_STREAM,0);
        _handler->selector = &selector;
    }

    void start()
    {
        Bind();
        Listen();
        selector.Add(_listen_sock);

        for(;;)
        {
            std::vector<int>fds;
            selector.Wait(&fds);//获取就绪事件
            Dispatcher(fds);//处理事件
        }
    }

    ~ReactorServer()
    {
        close(_listen_sock);
    }

protected:
    void Dispatcher(const std::vector<int>&fds)
    {
        for(const auto& fd:fds)
        {
            if(fd == _listen_sock)
            {
                Acceptor();
            }
            else
            {
                _handler->hanle_event(fd);//单线程就不搞读写分离了
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
        selector.Add(newfd);
    }

    void Bind()
    {
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
    std::shared_ptr<EventHandler> _handler;
    Selector selector;
};