#include "log.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>

Log log;

struct ThreadData
{
    struct sockaddr_in server;
    int sockfd;

};

void *recv_msg(void* args)
{
    char buffer[1024] = {0};
    struct ThreadData* td = (struct ThreadData*)args;

    while(true)
    {
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        ssize_t s = recvfrom(td->sockfd,buffer,1023,0,(struct sockaddr*)&temp,&len);
        if(s>0)
        {
            buffer[s] = 0;
            std::cout<<buffer<<std::endl;
        }
    }
}

void *send_msg(void* args)
{
    struct ThreadData* td = (struct ThreadData*)args;
    std::string msg;
    socklen_t len = sizeof(td->server);
    while(true)
    {
        std::cout<<"Please Enter@";
        std::getline(std::cin,msg);
        //send msg
        sendto(td->sockfd,msg.c_str(),msg.size(),0,(struct sockaddr*)&td->server,len);
    }
}

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        std::cout<<"\n\rUsage:"<<argv[0]<<" serverip serverport\n";
        return 1;
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    struct ThreadData td;

    struct sockaddr_in server;
    bzero(&server,sizeof(server));
    td.server.sin_family = AF_INET;
    td.server.sin_port = htons(serverport);
    td.server.sin_addr.s_addr = inet_addr(serverip.c_str());
    td.sockfd = socket(AF_INET,SOCK_DGRAM,0);

    if(td.sockfd<0)
    {
        std::cout<<"socket err\n"<<std::endl;
    }

    pthread_t recvr,sender;
    pthread_create(&recvr,nullptr,recv_msg,&td);
    pthread_create(&sender,nullptr,send_msg,&td);

    pthread_join(recvr,nullptr);
    pthread_join(sender,nullptr);


    close(td.sockfd);
    return 0;
}