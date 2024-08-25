#include "selectServer.hpp"
#include <memory>

int main()
{
    std::unique_ptr<SelectServer> svr(new SelectServer());
    svr->Init();
    svr->Start();
    return 0;
}