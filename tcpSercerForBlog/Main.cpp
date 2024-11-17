
#include <iostream>
#include <memory>
#include "TCPEchoServer.hpp"


int main()
{
    std::shared_ptr<TCPEchoServer> ptr(new TCPEchoServer("127.0.0.1",8888));
    ptr->run();
    return 0;
}