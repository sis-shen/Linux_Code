#include "TCPSocket.hpp"
#include <iostream>
#include <thread>
using namespace std;

void Recv(void* arg)
{
    int fd = *(int*)arg;
    while(true)
    {
        char buffer[128];
        int n = read(fd,buffer,sizeof(buffer)-1);
        if(n == 0){printf("disconnected\n");break;}
        if(n<0){printf("read error\n");break;}

        buffer[n] = '\0';
        cout<<buffer<<endl<<endl;
    }
}

void Send(void* arg)
{
    int fd = *(int*)arg;
    string line;
    while(true)
    {
        getline(cin,line);
        int n = write(fd,line.c_str(),line.size());
        if(n == 0)
        {
            printf("disconnected\n");
            break;
        }
        if(n<0)
        {
            printf("write error\n");
            break;
        }
    }
}

int main()
{
    TCPSocket conSocket;
    conSocket.Socket();
    int fd = conSocket.Connect("127.0.0.1",8888);

    //使用C++线程库
    thread t1(Recv,&fd);
    thread t2(Send,&fd);

    t1.join();
    t2.join();

    return 0;
}