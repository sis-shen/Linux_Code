#pragma once
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include "log.hpp"

extern Log log;

using namespace std;

class Task
{
public:
    Task(int sockfd,const std::string&clientip,const uint16_t& clientport)
    :_sockfd(sockfd),_clientip(clientip),_clientport(clientport)
    {}
    ~Task(){}

    void run()
    {
        char buffer[4096];
        ssize_t n = read(_sockfd,buffer,sizeof(buffer));
        if(n>0)
        {
            buffer[n] = 0;
            std::cout<<"client say# "<<buffer<<std::endl;
            std::string echo_string = "tcpserver echo# ";
            echo_string+=buffer;
            write(_sockfd,echo_string.c_str(),echo_string.size());
        }
        else if(n == 0)
        {
            log(Info,"Connection closed, close fd: %d",_sockfd);
            return;
        }
        else 
        {
            log(Warning,"[%s:%u][sockfd: %d] read error",_clientip.c_str(),_clientport,_sockfd);
            return;
        }
        close(_sockfd);

    }
    void operator()()
    {
        run();
    }

private:
    int _sockfd;
    std::string _clientip;
    uint16_t _clientport;
};