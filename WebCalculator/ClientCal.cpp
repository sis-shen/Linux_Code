#include <iostream>
#include "Socket.hpp"
#include <unistd.h>
#include "Protocol.hpp"

const uint16_t clientport = 25565;
std::string clientip = "47.99.48.121";

int main()
{
    Sock sockfd;
    sockfd.Socket();
    bool r = sockfd.Connect(clientip,clientport);
    if(!r)
    {
        return 1;
    }
    srand(time(nullptr)^getpid());
    int cnt = 10;
    std::string opers = "+-*/%=&";

    while(cnt--)
    {
        int x = rand() % 100+1;
        usleep(1234);
        int y = rand()%100;
        usleep(4321);
        char op = opers[rand()%opers.size()];

        Request req(x,y,op);

        std::string package;
        req.Serialize(&package);

        package = Encode(package);

        std::cout<<"报文\n"<<package;
        write(sockfd.Fd(),package.c_str(),package.size());

        char buffer[128] = {0};
        ssize_t n = read(sockfd.Fd(),buffer,sizeof(buffer));
    }

    sockfd.Close();
    return 0;
}