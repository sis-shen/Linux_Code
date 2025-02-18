#include "PollServer.hpp"

int main()
{
    PollServer svr(8818);
    svr.init();
    svr.start();
    return 0;
}