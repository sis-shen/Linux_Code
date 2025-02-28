#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unordered_map>
#include <string>
#include <sys/epoll.h>
#include <shared_mutex>
#include "Selector.hpp"
class EventHandler
{
public:
    virtual void handle_read(int fd) = 0;
    virtual void handle_write(int fd) = 0;
    virtual ~EventHandler() = default;

    std::unordered_map<int,std::string> _buffer_hash;
    std::unordered_map<int,std::shared_mutex> _fd_mutex;
    Selector* selector;
};

class EchoHandler:public EventHandler
{
public:
    void handle_read(int fd)override
    {
        std::unique_lock<std::shared_mutex> lock(getFdMutex(fd));//写锁
        char buffer[1024] = {0};
        int ret = read(fd,buffer,1024-1);
        if(ret == 0)
        {
            //连接关闭
            close(fd);
            selector->Del(fd);
        }
        else if (ret < 0)
        {
            //连接错误
            perror("read fd");
            close(fd);
            selector->Del(fd);
        }
        buffer[ret] = '\0';
        printf("Get a msg@ %s",buffer);
        //再写回
        ssize_t n = send(fd,buffer,sizeof(buffer),MSG_DONTWAIT);//非阻塞发送
        if(n == -1)
        {
            //如果发送失败
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                //发送缓冲区已满，注册 EPOLLOUT等待可写事件
                struct epoll_event ev;
                ev.data.fd = fd;
                ev.events= EPOLLIN | EPOLLOUT | EPOLLET;
                selector->Mod(fd,ev);//注册侦听写事件就绪
                std::string& buf_str = _buffer_hash[fd];
                buf_str.append(buffer);//写入缓冲区
            }
            else 
            {
                //无法处理则关闭连接
                close(fd);
                eraseFdMutex(fd);
                selector->Del(fd);
            }
        }
    }

    void handle_write(int fd)override
    {
        std::unique_lock<std::shared_mutex> lock(getFdMutex(fd));//写锁
        auto&str = _buffer_hash[fd];
        ssize_t n = send(fd,str.c_str(),str.size(),MSG_DONTWAIT);//非阻塞发送
        if(n == -1)
        {
            //如果发送失败
            if(errno == EAGAIN || errno == EWOULDBLOCK)
            {
                //什么也不做，等待下次就绪
                //发送缓冲区已满，注册 EPOLLOUT等待可写事件
                struct epoll_event ev;
                ev.data.fd = fd;
                ev.events= EPOLLIN | EPOLLOUT | EPOLLET;
                selector->Mod(fd,ev);//注册侦听写事件就绪
                return;
            }
            else 
            {
                //无法处理则关闭连接
                close(fd);
                eraseFdMutex(fd);
                selector->Del(fd);
            }
        }

        if (n > 0) {
            str.erase(0, n); // 删除已发送部分
            if (str.empty()) {
                _buffer_hash.erase(fd);
                struct epoll_event ev;
                ev.data.fd = fd;
                ev.events= EPOLLIN | EPOLLET;
                selector->Mod(fd,ev);
                // 取消 EPOLLOUT 监听
            } else {
                // 保留未发送数据，等待下次写入
                struct epoll_event ev;
                ev.data.fd = fd;
                ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
                selector->Mod(fd, ev);
            }
        }
        
    }

    std::shared_mutex& getFdMutex(int fd)
    {
        std::unique_lock<std::mutex> lock(_map_mutex);//使对锁表的查询互斥
        return _fd_mutex[fd];//若锁不存在，则会自动构造
    }

    void eraseFdMutex(int fd)
    {
        std::unique_lock<std::mutex> lock(_map_mutex);//使对锁表的查询互斥
        _fd_mutex.erase(fd);
    }
private:
    std::mutex _map_mutex;
};