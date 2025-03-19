#include <iostream>
using namespace std;

void test1()
{
    cout<<"test 1 "<<endl;
}

int main()
{
    for(int i = 0;i<5;++i)
    {
        cout<<"what?"<<i<<endl;
    }
    cout<<"main::1"<<endl;
    test1();
    cout<<"main::2"<<endl;
    return 0;
}