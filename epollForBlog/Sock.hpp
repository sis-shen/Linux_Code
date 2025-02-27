#pragma once

#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <string>

enum{
    SOCK_ERR = 2
    ,BIND_ERR
    ,LISTEN_ERR
    ,ACCEPT_ERR
};

class Sock
{
public:
    Sock(){}
    Sock(int fd):_sockfd(fd){}
    ~Sock(){}
public:

    void Socket()
    {
        _sockfd = socket(AF_INET,SOCK_STREAM,0);
        if(_sockfd < 0)
        {
            printf("[Fatal]socket error,errno: %d error string:%s\n",errno,strerror(errno));
            exit(SOCK_ERR);
        }
        int opt = 1;
        setsockopt(_sockfd,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt));
    }
    void Bind(uint16_t port)
    {  
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if(bind(_sockfd,(const struct sockaddr*)&local,sizeof(local)) < 0)
        {
            printf("[Fatal]bind error,errno: %d error string:%s\n",errno,strerror(errno));
            exit(BIND_ERR);
        }   
    }

    void Listen(int backlog = 5)
    {
        if(listen(_sockfd,backlog) < 0)
        {
            printf("[Fatal]listen error,errno: %d error string:%s\n",errno,strerror(errno));
            exit(LISTEN_ERR);
        }
    }
    int Accept(std::string* clientip,uint16_t* clienport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int newfd = accept(_sockfd,(struct sockaddr*)&peer,&len);
        if(newfd < 0)
        {
            printf("[Fatal]accept error,errno: %d error string:%s\n",errno,strerror(errno));
            return -1;
        }
        char ipstr[64] = {0};
        inet_ntop(AF_INET,&peer.sin_addr,ipstr,sizeof(ipstr));
        *clientip = ipstr;
        *clienport = ntohs(peer.sin_port);
        return newfd;
    }
    bool Connect(const std::string& ip,uint16_t port)
    {
        struct sockaddr_in peer;
        memset(&peer,0,sizeof(peer));
        peer.sin_family =AF_INET;
        peer.sin_port = htons(port);
        inet_pton(AF_INET,ip.c_str(),&(peer.sin_addr));

        int n = connect(_sockfd,(struct sockaddr*)&peer,sizeof(peer));
        if(n == -1) 
        {
            std::cerr<<"connect to "<<ip<<":"<<port<<std::endl;
            return false;
        }

        return true;

    }

    int Recv(std::string& req)
    {
        char buffer[1024] = {0};
        int ret = read(_sockfd,buffer,1024-1);
        if(ret == 0)
        {
            //连接关闭
            return 0;
        }
        else if(ret < 0)
        {
            //发生错误
            perror("read");
            return ret;
        }
        else
        {
            //正确读取
            req = std::string(buffer);
        }
        return ret;
    }

    int Send(const std::string&res)
    {
        return write(_sockfd,res.c_str(),res.size());
    }

    void Close()
    {
        close(_sockfd);
    }

    int Fd() const
    {
        return _sockfd;
    }
private:
    int _sockfd;
};