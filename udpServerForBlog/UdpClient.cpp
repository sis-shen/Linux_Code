
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>


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

int main()
{
    //硬编码服务器信息
    std::string serverip("127.0.0.1");//环回地址
    uint16_t serverport = 25565;

    struct ThreadData td;

    //准备td数据包
    bzero(&td.server,sizeof(td.server));
    td.server.sin_family = AF_INET;//使用IPV4协议
    td.server.sin_port = htons(serverport);//端口
    td.server.sin_addr.s_addr = inet_addr(serverip.c_str());//地址
    td.sockfd = socket(AF_INET,SOCK_DGRAM,0);//创建套接字

    if(td.sockfd<0)
    {
        std::cout<<"socket err\n"<<std::endl;
        return -1;//套接字创建失败
    }

    pthread_t recvr,sender;
    pthread_create(&recvr,nullptr,recv_msg,&td);//创建收消息线程
    pthread_create(&sender,nullptr,send_msg,&td);//创建发消息线程

    pthread_join(recvr,nullptr);
    pthread_join(sender,nullptr);


    close(td.sockfd);
    return 0;
}