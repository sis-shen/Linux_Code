#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main()
{
    int fd = open("log.txt",O_RDWR|O_CREAT|O_TRUNC|O_APPEND);//追加模式打开一个新文件

    pid_t id = fork();//创建子进程
    if(id == 0)
    {
        dup2(fd,1);//只有子进程重定向了标准输出
        execvp("./execute",NULL);//进程替换
        exit(0);
    }
    else 
    {
        waitpid(id,0,0);//阻塞等待子进程
        printf("parent wait success\n");//父进程输出
    }

    return 0;
}