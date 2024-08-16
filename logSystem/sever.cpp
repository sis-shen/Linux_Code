#include "log.hpp"

int main()
{
    Log log(MultiFile);
    log(Info,"everything fine");
    return 0;
}