#include "EpollServer.hpp"

int main()
{
    EpollServer svr(8111);
    svr.Start();
    return 0;
}