#include <unistd.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <iostream>

#define PAGE_SIZE 4096

int main(int argc,char* argvs[])
{
    if(argc != 2)
    {
        std::cout<<"Usage:"<< argvs[0] <<"filename"<<std::endl;
        return 1;
    }
    //1.打开目标文件
    int fd = open(argvs[1],O_RDWR|O_CREAT|O_TRUNC,0666);
    if(fd < 0)
    {
        std::cout<<"打开文件失败"<<std::endl;
        return 2;
    }

    //2.手动调整文件的大小
    if(ftruncate(fd,PAGE_SIZE) == -1 )
    {
        std::cout<<"Failed to truncate file: " << argvs[1]<<std::endl;
        return 3;
    }
    //3.开启文件映射
    char *shmaddr =(char*) ::mmap(NULL,PAGE_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(shmaddr == MAP_FAILED)
    {
        std::cout<<"mmap失败"<<std::endl;
        return 4;
    }

    //4.进行文件操作
    for(int ch = 'a';ch<='z';++ch)
    {
        shmaddr[ch-'a'] = ch;
        sleep(1);
    }

    //5.关闭文件映射
    if(::munmap(shmaddr,PAGE_SIZE) == -1)
    {
        std::cout<<"munmap失败:"<< argvs[1] <<std::endl;
        return 5;
    }

    close(fd);

    return 0;
}