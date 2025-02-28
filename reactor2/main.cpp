#include "Reactor.hpp"

int main()
{
    ReactorServer rsvr(8111);
    rsvr.start();
    return 0;
}