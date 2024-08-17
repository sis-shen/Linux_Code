#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <string>
void Usage(const std::string& proc)
{
    std::cout<<"\n\rUsage: "<<proc << " server_ip server_port";
}

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);


    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_pton(AF_INET,serverip.c_str(),&(server.sin_addr));
    server.sin_port = htons(serverport);


    std::string line;

    while(true)
    {
        int sockfd = -1;
        int cnt = 5;
        bool is_reconnect = false;
        do
        {
            sockfd = socket(AF_INET,SOCK_STREAM,0);
            if(sockfd < 0)
            {
                std::cout<<"socket error"<<std::endl;
                return 1;
            }

            int n = connect(sockfd,(const struct sockaddr*)&server,sizeof(server));
            if(n<0)
            {
                cnt--;
                is_reconnect = true;
                std::cerr<<"connetion failed...,reconnect count backwards: "<<cnt<<std::endl;
                sleep(1);
            }
            else 
            {
                break;
            }
        } while (cnt && is_reconnect);
        if(cnt == 0)
        {
            std::cout<<"user offline"<<std::endl;
            break;
        }
        std::cout<<"Please Enter@ ";
        std::getline(std::cin,line);
        write(sockfd,line.c_str(),line.size());

        char inbuffer[4096];
        int n = read(sockfd,inbuffer,sizeof(inbuffer));
        if(n>0)
        {
            inbuffer[n] = 0;
            std::cout<<inbuffer<<std::endl;
        }
        else 
        {
            is_reconnect = true;
            std::cout<<"read err"<<std::endl;
            continue;
        }
        close(sockfd);
    }

    return 0;
}