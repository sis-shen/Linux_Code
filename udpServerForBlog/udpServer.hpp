#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <unordered_map>

std::string defualt_ip = "0.0.0.0";//接收任意地址的可用udp数据包-对外提供服务的ip地址
uint16_t defualt_port = 25565; //硬编码一个使用端口号
const int size = 1024; //规定缓冲区大小

enum{
    SOCKET_ERR = 1,
    BIND_ERR
};

class UdpServer
{    
public:
    UdpServer(uint16_t port = defualt_port,const std::string& ip = defualt_ip )
        :_port(port),_ip(ip),_isRunning(false)
    {}

    void Init()
    {
        _sockfd  = socket(AF_INET,SOCK_DGRAM,0);//创建套接字
        if(_sockfd < 0)
        {
            printf("[Fatal],socket creation failed,sockfd: %d\n",_sockfd);
            exit(SOCKET_ERR);
        }
        printf("[Info],socket create succeeded , sockfd: %d\n",_sockfd);

        //bind socket
        //准备sockaddr_in的内容
        struct sockaddr_in local;
        bzero(&local,sizeof(local));//先置0
        local.sin_family = AF_INET; //指定IP协议为IPV4
        local.sin_port = htons(_port);;//指定端口号，要使用hton系类函数转换成网络字节序
        // local.sin_addr.s_addr = inet_addr(_ip.c_str());
        local.sin_addr.s_addr = INADDR_ANY;//和ip == '0.0.0.0'的效果是一样的

        //绑定并判断是否成功
        if(bind(_sockfd,(const struct sockaddr *)& local,sizeof(local)) < 0)
        {
            printf("Fatal,bind error,errno:%d ,err string: %s\n",errno,strerror(errno));
        }
        //打印成功的日志
        printf("Info,%s:%u bind succeeded\n",_ip.c_str(),_port);
    }

    void CheckUser(const struct sockaddr_in& client,const std::string &clientip,uint16_t clientport)
    {
        auto iter = _ol_usr.find(clientip);
        if(iter == _ol_usr.end())
        {
            printf("Info,[%s] added to oline user\n",clientip.c_str());
            _ol_usr.insert({clientip,client});
        }
    }

    void Broadcast(const std::string& info,const std::string &clientip,uint16_t clientport)
    {
        std::string msg = "[";
        msg+=clientip;
        msg+=":";
        msg+=std::to_string(clientport);
        msg+="]# ";
        msg+= info;
        for(const auto&user:_ol_usr)
        {
            printf("Info,Broadcast\n");
            socklen_t len = sizeof(user.second);
            sendto(_sockfd,msg.c_str(),msg.size(),0,(const sockaddr*)&(user.second),len);
        }
    }

    void Run()
    {
        printf("Info,Server started running...\n");
        _isRunning = true;
        char inbuffer[size] = {0};
        while(_isRunning)
        {
            sleep(1);
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            ssize_t n = recvfrom(_sockfd,inbuffer,sizeof(inbuffer)-1,0,(struct sockaddr *) &client , &len);
            if(n<0)
            {
                printf("Warning,recvfrom error,errno:%d ,err string: %s\n",errno,strerror(errno));
            }
            inbuffer[n] = 0;

            uint16_t clientport = ntohs(client.sin_port);
            std::string clientip = inet_ntoa(client.sin_addr);

            std::string info = inbuffer;
            CheckUser(client,clientip,clientport);
            Broadcast(info,clientip,clientport);
            printf("Info, Server get a msg! [%s:%u]: %s\n",clientip.c_str(),clientport,inbuffer);
    
        }
    }

    ~UdpServer()
    {
        if(_sockfd>0)
        {
            close(_sockfd);
        }
    }

private:
    int _sockfd;
    bool _isRunning;

    uint16_t _port;
    std::string _ip;
    std::unordered_map<std::string,struct sockaddr_in> _ol_usr;
};