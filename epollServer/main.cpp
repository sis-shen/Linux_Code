#include <iostream>
#include "epollServer.hpp"

int main()
{
    std::shared_ptr<EpollServer> epoller_svr(new EpollServer());
    epoller_svr->Init();
    epoller_svr->Start();
    return 0;
}