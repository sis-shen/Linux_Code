
#include <memory>
#include <iostream>
#include "udpServer.hpp"

int main(int argc,char* args[])
{
    std::unique_ptr<UdpServer> svr(new UdpServer());//使用默认端口号和ip
    svr->Init();//初始化
    svr->Run();//启动运行
    return 0;
}