#pragma once

#include <sys/select.h> //提供select函数
#include <fcntl.h>      //提供文件控制
#include <iostream>     //提供输入输出流
#include "Socket.hpp"   
#include <string>       

static const uint16_t default_port = 8976;
static const uint16_t fd_num_max = sizeof(fd_set)*8;
const int default_fd = -1;

class SelectServer
{
public:
    SelectServer(uint16_t port = default_port)
    : _port(port)
    {
        for (int i = 0; i < fd_num_max; ++i)//初始化
        {
            _fd_array[i] = default_fd;
        }
    }

    ~SelectServer() { _listen_sock.Close(); }

    //初始化函数
    void init()
    {
        _listen_sock.Socket();//创建文件描述符
        _listen_sock.Bind(_port);//绑定监听端口
        _listen_sock.Listen();//开始监听
    }
    //负责调用监听链接请求 或 接收消息
    void dispatcher(fd_set & rfds)
    {
        for(int i = 0;i<fd_num_max;++i)
        {
            int fd = _fd_array[i];
            if(fd ==  default_fd)
            {
                //默认fd，说明该位置未被使用
                continue;
            }
            else if(FD_ISSET(fd,&rfds))
            {
                if(fd == _listen_sock.Fd())
                {
                    //监听文件描述符就绪，说明有新的连接请求
                    acceptor();
                }
                else
                {
                    //普通的网络文件描述符就绪，说明读就绪
                    recver(fd,i);
                }
            }
        }
    }
    //负责接收所有就绪的连接请求
    void acceptor()
    {
        std::string client_ip;
        uint16_t client_port = 0;
        int sock = _listen_sock.Accept(&client_ip,&client_port);
        if(sock<0)
            return;//连接失败
        //连接成功
        printf("[INFO]accepct success,[%s:%d]\n",client_ip.c_str(),client_port);

        int pos = fd_num_max;
        for(int i = 0;i<fd_num_max;++i)
        {
            if(_fd_array[i] == default_fd)
            {
                pos = i;//找到了一个空位
                break;
            }
        }

        if(pos < fd_num_max)
        {
            //确实找到了空位
            _fd_array[pos] = sock;
        }
        else 
        {
            printf("[Warning]连接数已超载，已丢弃本次链接 %d\n",sock);
            close(sock);//关闭连接
        }
    }
    //负责接收指定fd的信息
    void recver(int fd,int pos)
    {
        char buffer[1024] = {0};//接收缓冲区
        int n = read(fd,buffer,1024-1);//接收网络信息
        if(n>0)
        {
            //正常接收到消息
            printf("get a msg# %s\n",buffer);
        }
        else if (n == 0)
        {
            //收到链接关闭消息
            printf("[INFO] client quit. close fd: %d\n",fd);
            //回收资源
            close(fd);
            _fd_array[pos] = default_fd;
        }
        else
        {
            //出现异常
            printf("[Warning] read error,fd: %d\n",fd);
            //回收资源
            close(fd);
            _fd_array[pos] = default_fd;
        }
    }
    //启动函数
    void start()
    {
        //将监听文件描述符放入fd池中
        int fd = _listen_sock.Fd();
        _fd_array[0] = fd;

        for(;;)//死循环
        {
            fd_set rfds;//生成一个新的位图
            FD_ZERO(&rfds);//初始化清零
            struct timeval timeout = {1,0};//设置超时

            int maxfd = _fd_array[0];
            //设置需要select监听的文件描述符
            for(int i = 0;i<fd_num_max;++i)
            {
                if(_fd_array[i] == default_fd)
                {
                    //跳过空位
                    continue;
                }
                FD_SET(_fd_array[i],&rfds);
                if(maxfd < _fd_array[i])
                {
                    maxfd = _fd_array[i];//维护最大的被使用的文件描述符
                }
            }

            int n = select(maxfd+1,&rfds,nullptr,nullptr,&timeout);//开始监听
            switch(n)
            {
                case 0:
                    std::cout<<("timeout ! ") << timeout.tv_sec<<"."<<timeout.tv_usec<<std::endl;
                    break;
                case -1:
                    std::cerr<<"select error\n";//select出现错误
                    break;
                default:
                    printf("[INFO] get some link : %d",n);
                    dispatcher(rfds);//处理连接
                    break;
            }
        }
    }

private:
    Sock _listen_sock;
    uint16_t _port;
    int _fd_array[fd_num_max];
};