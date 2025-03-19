#include <iostream>
#include <string>
#include <sw/redis++/redis++.h>
int main()
{
    sw::redis::Redis redis("tcp://127.0.0.1:6379");

    std::string res = redis.ping();
    std::cout<<res<<std::endl;
    return 0;
}