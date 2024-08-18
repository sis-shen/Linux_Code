#pragma once

#include <unistd.h>
#include "Socket.hpp"
#include "log.hpp"
#include <signal.h>
#include <functional>
#include <string>


using func_t = std::function<std::string (std::string&package)>;

class TcpServer
{
public:
    TcpServer(uint16_t port,func_t callback)
    :_port(port),_callback(callback)
    {}
    ~TcpServer(){}
    bool Init()
    {
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
        log(Info,"server init done");
        return true;
    }

    void Start()
    {
        signal(SIGCHLD,SIG_IGN);
        while(true)
        {
            std::string clientip;
            uint16_t clientport;
            int sockfd = _listensock.Accept(&clientip,&clientport);
            log(Info,"accept a new link, sockfd:%d ,target: [%s:%u]",sockfd,clientip.c_str(),clientport);
            if(sockfd<0)
            {
                continue;
            }

            //提供服务
            if(fork() == 0)
            {
                _listensock.Close();
                while(true)
                {   
                    std::string inbuffer_stream = "";
                    char buffer[128] = {0};
                    ssize_t n = read(sockfd,buffer,sizeof(buffer) - 1);
                    if(n > 0)
                    {
                        buffer[n] = 0;
                        inbuffer_stream+=buffer;
                        std::string info = _callback(inbuffer_stream);
                        if(info.empty())
                        {
                            continue;
                        }
                        //write
                        write(sockfd,info.c_str(),info.size());
                    }
                    else if(n == 0)break;
                    else break;
                }

                exit(0);
            }
        }
    }
private:
    uint16_t _port;
    Sock _listensock;
    func_t _callback;
};