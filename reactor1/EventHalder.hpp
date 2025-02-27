#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Selector.hpp"
class EventHandler
{
public:
    virtual void hanle_event(int fd) = 0;
    virtual ~EventHandler() = default;

    Selector* selector;
};

//一次业务把读写都完成了
class EchoHandler:public EventHandler
{
public:
    void hanle_event(int fd)override
    {
        char buffer[1024] = {0};
        int ret = read(fd,buffer,1024-1);
        if(ret == 0)
        {
            //连接关闭
            close(fd);
            selector->Del(fd);
        }
        else if (ret < 0)
        {
            //连接错误
            perror("read fd");
            close(fd);
            selector->Del(fd);
        }

        printf("Get a msg@ %s",buffer);
        //再写回
        write(fd,buffer,ret);
    }

};