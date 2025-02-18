#include "selectServer.hpp"
#include <memory>

int main()
{
    std::unique_ptr<SelectServer> svr(new SelectServer());
    svr->init();
    svr->start();
    return 0;
}