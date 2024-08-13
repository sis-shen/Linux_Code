#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>

#define N 2
#define NUM 1024

using namespace std;

//child
void Writer(int wfd)
{
    string s = "hello,I am child";//准备子进程的报文
    pid_t self = getpid();
    int number = 5;

    char buffer[NUM] = {0};
    while(number--)
    {
        buffer[0] = 0;//清空字符串
        snprintf(buffer,sizeof(buffer),"%s-%d-%d\n",s.c_str(),self,number);

        //发送/写入报文
        write(wfd,buffer,strlen(buffer));

        sleep(1);
    }
}

//father
void Reader(int rfd)
{
    char buffer[NUM];

    while(true)
    {
        buffer[0] = 0;
        ssize_t n = read(rfd,buffer,sizeof(buffer));
        if(n > 0)
        {
            buffer[n] = 0;//恢复成字符串
            cout<<"father get a message >> "<<buffer<<endl;
        }
    }
}

int main()
{
    int pipefd[N] = {0};
    
    int n = pipe(pipefd);
    if(n<0) return 1;

    pid_t id = fork();
    if(id  < 0) return 2;
    else if(id == 0)
    {
        //子进程
        close(pipefd[0]);//关闭读
        
        Writer(pipefd[1]);

        close(pipefd[1]);
        exit(0);
    }
    else
    {
        close(pipefd[1]);//关闭写

        Reader(pipefd[0]);

        pid_t rid = waitpid(id,nullptr,0);

        if(id <0) return 3;

        close(pipefd[0]);
    }

    return 0;
}