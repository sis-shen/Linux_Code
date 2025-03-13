#include <iostream>

void test1()
{
    double j = 2.0;
    for(int i = 0;i<1000000000;++i)
    {
        j += 1.0+2.111;
    }
}

void test2()
{
    for(int i = 0;i<50;++i)
    {
        test1();
    }
}

void test3()
{
    for(int  i = 0;i<10;++i) test1();
    test2();
}

int main()
{
    test3();
    return 0;
}