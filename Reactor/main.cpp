#include "TcpServer.hpp"
#include <memory>

int main()
{
    std::shared_ptr<TcpServer> tcp_svr(new TcpServer());
    tcp_svr->Init();
    tcp_svr->Start();

    return 0;
}