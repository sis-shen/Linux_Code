#include "TcpServer.hpp"
#include <iostream>
#include <memory>
#include <pthread.h>


int main()
{
    std::unique_ptr<TcpServer> tsr(new TcpServer());
    tsr->Init();
    tsr->Start();
    return 0;
}