#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    printf("hello1 ");//父进程向标准输出打印一句话
    fprintf(stdout,"hello2 ");//父进程向标准输出打印一句话

    fork();
    return 0;
}