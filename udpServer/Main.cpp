#include "udpServer.hpp"
#include <memory>

Log log(Screen);

int main(int argc,char* args[])
{
    if(argc != 2)
    {
        std::cout<<"Wrong use\n\t"<<"./udpserver port";
    }
    std::unique_ptr<UdpServer> svr(new UdpServer(std::stoi(args[1])));
    svr->Init();
    svr->Run();
    return 0;
}