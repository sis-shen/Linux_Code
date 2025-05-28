#include <iostream>
#include <memory>
#include <string>
#include "HttpServer.hpp"

extern Log log;

int main()
{
    std::unique_ptr<HttpServer> svr(new HttpServer());
    svr->Start();
    return 0;
}