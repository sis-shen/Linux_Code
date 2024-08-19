#include "Socket.hpp"
#include "Protocol.hpp"
#include "TcpServer.hpp"
#include "ServerCal.hpp"
#include "daemon.hpp"
int main()
{
    ServerCal cal;
    TcpServer *tsvp = new TcpServer(25565,std::bind(&ServerCal::Calculator,&cal,std::placeholders::_1));

    tsvp->Init();
    //Daemon();
    //daemon(0,0);//守护进程
    tsvp->Start();
    

    return 0;
}