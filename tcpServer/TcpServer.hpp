#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "log.hpp"
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "Threadpool.hpp"
#include "Task.hpp"
#include "daemon.hpp"

Log log(MultiFile);

enum{
    SOCK_ERR = 2,
    BIND_ERR,
    LISTEN_ERR
};

const int default_sockfd = -1;
const uint16_t default_port = 25565;
std::string defualt_ip = "0.0.0.0";
const int backlog = 5;//不要设置太大

class TcpServer;

struct ThreadData
{
public:
    ThreadData(int fd,const std::string& ip,const uint16_t& port,TcpServer *ptr)
    :sockfd(fd),clientip(ip),clientport(port),tsvr(ptr)
    {}
public:
    int sockfd;
    std::string clientip;
    uint16_t clientport;
    TcpServer *tsvr;
};

class TcpServer
{
public:
    TcpServer(const uint16_t port = default_port,const std::string& ip = defualt_ip)
    :_listen_sockfd(default_sockfd),_port(port),_ip(ip)
    {}

    ~TcpServer(){}

    void Init()
    {
        signal(SIGCHLD,SIG_IGN);
        _listen_sockfd = socket(AF_INET,SOCK_STREAM,0);
        if(_listen_sockfd < 0)
        {
            log(Fatal,"socket failed, errno: %d, errtstring%s",errno,strerror(errno));;
            exit(SOCK_ERR);
        }

        log(Info,"socket succeeded, _sockfd: %d",_listen_sockfd);

        int opt = 1;
        setsockopt(_listen_sockfd,SOL_SOCKET,SO_REUSEADDR |SO_REUSEPORT,&opt,sizeof(opt));
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        inet_aton(_ip.c_str(),&(local.sin_addr));

        //bind
        if(bind(_listen_sockfd,(const struct sockaddr*)&local,sizeof(local) ) < 0)
        {
            log(Fatal,"bind error,errno: %d , errorstring: %s",errno,strerror(errno));
            exit(BIND_ERR);
        }
        log(Info,"bind succeeded");
        //等待链接
        if(listen(_listen_sockfd,backlog) < 0)
        {
            log(Fatal,"listen error,errno: %d , errorstring: %s",errno,strerror(errno));
            exit(LISTEN_ERR);
        }
        log(Info,"listen succeeded");

    }

    void Start()
    {
        Daemon();
        ThreadPool<Task>::Getinstance()->Start();
        log(Info,"tcp sever is running...");
        for(;;)
        {
            struct sockaddr_in client;
            socklen_t len = sizeof(len);
            int sockfd = accept(_listen_sockfd,(struct sockaddr *)&client,&len);
            if(sockfd < 0)
            {
                log(Error,"accept error,errno: %d , errorstring: %s",errno,strerror(errno));
                continue;
            }
            log(Info,"get a new link");
            
            uint16_t clientport = ntohs(client.sin_port);
            char clientip[32] = {0};
            inet_ntop(AF_INET,&(client.sin_addr),clientip,sizeof(clientip));
            //1.version 1 单进程
            // Service(sockfd,clientip,clientport);
            // close(sockfd);

            //2.多进程版本
            // pid_t id = fork();
            // if(id = 0)
            // {
            //     close(_listen_sockfd);
            //     // if(fork() > 0) exit(0);//孙子进程，（立马会子进程退出，孙子进程变成孤儿进程）
            //     Service(sockfd,clientip,clientport);   
            //     close(sockfd);
            //     exit(0);
            // }
            // close(sockfd);
            // pid_t rid = waitpid(id,nullptr,0);
            // (void) rid;

            //3.多线程
            // pthread_t tid;
            // ThreadData* td = new ThreadData(sockfd,clientip,clientport,this);
            // pthread_create(&tid,nullptr,Routine,td);

            //4.线程池
            Task task(sockfd,clientip,clientport);
            ThreadPool<Task>::Getinstance()->Push(task);
        }
    }

    static void*Routine(void* args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData*>(args);
        td->tsvr->Service(td->sockfd,td->clientip,td->clientport);   
        delete td;
        return nullptr;
    }

    void Service(int sockfd,const std::string&clientip,const uint16_t& clientport)
    {
        char buffer[4096];
        while(true)
        {
            ssize_t n = read(sockfd,buffer,sizeof(buffer));
            if(n>0)
            {
                buffer[n] = 0;
                std::cout<<"client say# "<<buffer<<std::endl;
                std::string echo_string = "tcpserver echo# ";
                echo_string+=buffer;
                write(sockfd,echo_string.c_str(),echo_string.size());
            }
            else if(n == 0)
            {
                log(Info,"Connection closed, close fd: %d",sockfd);
                break;
            }
            else 
            {
                log(Warning,"[%s:%u][sockfd: %d] read error",clientip.c_str(),clientport,sockfd);
                break;
            }
        }

    }

private:
    int _listen_sockfd;
    uint16_t _port;
    std::string _ip;
};