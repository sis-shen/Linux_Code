#pragma once

#include "log.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <unordered_map>

std::string defualt_ip = "0.0.0.0";//任意地址绑定
uint16_t defualt_port = 8080;
const int size = 1024;


extern Log log;

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
        _sockfd  = socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd < 0)
        {
            log(Fatal,"socket creation failed,sockfd: %d",_sockfd);
            exit(SOCKET_ERR);
        }
        log(Info,"socket create succeeded , sockfd: %d",_sockfd);

        //bind socket
        struct sockaddr_in local;
        bzero(&local,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);;
        // local.sin_addr.s_addr = inet_addr(_ip.c_str());
        local.sin_addr.s_addr = INADDR_ANY;

        if(bind(_sockfd,(const struct sockaddr *)& local,sizeof(local)) < 0)
        {
            log(Fatal,"bind error,errno:%d ,err string: %s",errno,strerror(errno));
        }
        log(Info,"%s:%u bind succeeded",_ip.c_str(),_port);
    }

    void CheckUser(const struct sockaddr_in& client,const std::string &clientip,uint16_t clientport)
    {
        auto iter = _ol_usr.find(clientip);
        if(iter == _ol_usr.end())
        {
            log(Info,"[%s] added to oline user",clientip.c_str());
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
            log(Info,"Broadcast");
            socklen_t len = sizeof(user.second);
            sendto(_sockfd,msg.c_str(),msg.size(),0,(const sockaddr*)&(user.second),len);
        }
    }

    void Run()
    {
        log(Info,"Server started running...");
        _isRunning = true;
        char inbuffer[size] = {0};
        while(_isRunning)
        {
            sleep(1);
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            size_t n = recvfrom(_sockfd,inbuffer,sizeof(inbuffer)-1,0,(struct sockaddr *) &client , &len);
            if(n<0)
            {
                log(Warning,"recvfrom error,errno:%d ,err string: %s",errno,strerror(errno));
            }
            inbuffer[n] = 0;

            uint16_t clientport = ntohs(client.sin_port);
            std::string clientip = inet_ntoa(client.sin_addr);

            std::string info = inbuffer;
            CheckUser(client,clientip,clientport);
            Broadcast(info,clientip,clientport);
            log(Info," Server get a msg! [%s:%u]: %s",clientip.c_str(),clientport,inbuffer);
    
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