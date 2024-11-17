#pragma once 
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

//封装一个使用IPV4面向字节流的,专门用于监听和连接的TCP套接字类
class TCPSocket
{
public:
    TCPSocket(int fd=-1)
    :_sock_fd(fd){}

    bool Socket()//创建套接字
    {
        _sock_fd = socket(AF_INET,SOCK_STREAM,0);
        if(_sock_fd < 0)
        {
            perror("socket");
            return false;
        }
        printf("info: open socket fd :%d\n",_sock_fd);
        return true;
    }

    bool Bind(const std::string& ip,uint16_t port) const
    {
        struct sockaddr_in addr_in;
        addr_in.sin_family = AF_INET;
        addr_in.sin_addr.s_addr = inet_addr(ip.c_str());
        addr_in.sin_port = htons(port);

        int ret = bind(_sock_fd,(const struct sockaddr*)&addr_in,sizeof(addr_in));
        if(ret<0)
        {
            perror("bind");
            return false;
        }

        return true;
    }

        //开始监听
    bool Listen(int backlog = 5)const
    {
        int ret = listen(_sock_fd,backlog);
        if(ret < 0)
        {
            perror("listen");
            return false;
        }
        return true;
    }

    int Accept(std::string* client_ip=nullptr,uint16_t* client_port = nullptr)
    {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        int new_fd = accept(_sock_fd,(struct sockaddr*)&client_addr,&len);
        if(new_fd < 0)
        {
            perror("accept");
            return new_fd;
        }

        printf("info, accept fd: %d\n",new_fd);

        if(client_ip !=nullptr)
        {
            char buffer[128] = {0};
            inet_ntop(AF_INET,&client_addr,buffer,sizeof(client_addr));
            *client_ip = std::string(buffer);
        }
        if(client_port != nullptr)
        {
            *client_port = ntohs(client_addr.sin_port);
        }
        return new_fd;//返回新建的文件
    }

    int Connect(const std::string&server_ip,const uint16_t server_port)
    {
        if(server_ip.empty())
        {
            printf("ip不可为空\n");
            return -1;
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(server_port);
        server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
        
        int ret = connect(_sock_fd,(const struct sockaddr*)&server_addr,sizeof(server_addr));

        if(ret<0)
        {
            perror("connect");
        }

        return ret;
    }

    void Close()
    {
        close(_sock_fd);
    }

    ~TCPSocket()
    {
        Close();
    }

private:
    int _sock_fd;
};