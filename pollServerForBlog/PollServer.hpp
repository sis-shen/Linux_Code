#pragma once

#include <sys/fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <poll.h>
#include <string.h>
#include <vector>
#include "Socket.hpp"

static const uint16_t defaultport = 25565;
static const uint16_t fd_num_max = 8;
static struct pollfd default_fd = {-1,0,0};

class PollServer
{
public:
    PollServer(const uint16_t port)
    :_port(port)
    {}

    ~PollServer(){close(_listen_sock.Fd());};

    //初始化函数
    void init()
    {
        _listen_sock.Socket();
        _listen_sock.Bind(_port);
        _listen_sock.Listen();
    }
    //负责调用监听链接请求 或 接收消息
    void dispatcher()
    {
        std::vector<int> del_list;
        int size = _fd_array.size();
        for(int i = 0;i<size;++i)
        {
            struct pollfd fd = _fd_array[i];
            if(fd.fd == default_fd.fd)
            {
                continue;
            }
            if(fd.fd == _listen_sock.Fd() == fd.revents == POLLIN)
            {
                acceptor();
            }
            else if (fd.revents == POLLIN)
            {
                printf("recver\n");
                recver(fd,i,del_list);
            }
        }
        for(int i = del_list.size()-1;i>=0;--i)
        {
            //从后向前删
            printf("已删除连接fd : %d \n",_fd_array[i].fd);
            _fd_array.erase(_fd_array.begin() + del_list[i]);
        }
    }
    //负责接收所有就绪的连接请求
    void acceptor()
    {
        std::string client_ip;
        uint16_t client_port;
        int sock = _listen_sock.Accept(&client_ip,&client_port);
        if(sock < 0)
            return;
        printf("[Info]accept success,[%s,%d]\n",client_ip.c_str(),client_port);
        
        _fd_array[0].revents = 0;//重置状态
        if(_fd_array.size() < fd_num_max)
        {
            _fd_array.push_back({sock,POLLIN,0});
        }
        else 
        {
            printf("[Warning] 连接数超载，已丢弃本次连接 fd :%d",sock);
            close(sock);
        }
    }
    //负责接收指定fd的信息
    void recver(struct pollfd& fd,int pos,std::vector<int>& del_list)
    {
        char buffer[1024] = {0};
        int n = read(fd.fd,buffer,1024-1);
        if(n>0)
        {
            fd.revents = 0;//重置状态
            std::cout<<"poll get msgs#" << buffer << std::endl;
        }
        else if(n == 0)
        {
            //连接断开
            printf("[INFO] client quit,close fd : %d",fd.fd);
            close(fd.fd);
            del_list.push_back(pos);
        }
        else 
        {
            printf("[Warning] read error, fd: %d",fd.fd);
            close(fd.fd);
            del_list.push_back(pos);
        }
    }
    //启动函数
    void start()
    {
        int listenfd = _listen_sock.Fd();
        _fd_array.push_back({listenfd,POLLIN,0});
        int cnt = 0;
        for(;;)
        {
            //DEBUG
            for(auto& fd : _fd_array)
            {
                printf("%d ",fd.fd);
            }printf("\n");
            int ret = poll(_fd_array.data(),_fd_array.size(),1000);
            if(ret < 0)
            {
                //发生错误
                perror("poll");
                continue;
            }
            else if (ret == 0)
            {
                printf("[INFO] poll time out %d\n",cnt++);
                continue;
            }
            else
            {
                dispatcher();
                continue;
            }
        }
    }

private:
    Sock _listen_sock;
    std::vector<struct pollfd> _fd_array;
    uint16_t _port;
};