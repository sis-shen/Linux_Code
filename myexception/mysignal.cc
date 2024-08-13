#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void myhandler(int signo)
{
    cout<<"FPE! signal: "<<signo<<endl;
    //exit(2); //先不退出
}

int main()
{
    signal(8,myhandler);
    cout<<"dev before"<<endl;
    sleep(1);
    int a =10;
    a /=0;//异常

    cout<<"dev after"<<endl;
    return 0;
}