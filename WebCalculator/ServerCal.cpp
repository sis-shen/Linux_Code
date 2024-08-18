#include "Socket.hpp"
#include "Protocol.hpp"
#include "TcpServer.hpp"
#include "ServerCal.hpp"
int main()
{
    ServerCal cal;
    TcpServer *tsvp = new TcpServer(25565,std::bind(&ServerCal::Calculator,&cal,std::placeholders::_1));

    tsvp->Init();
    tsvp->Start();
    

    return 0;
}