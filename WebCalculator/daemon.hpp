#pragma once

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


const std::string nullfile = "/dev/null";

void Daemon(const std::string& cwd = "")
{
    //1.忽略部分信号
    signal(SIGCLD,SIG_IGN);
    signal(SIGPIPE,SIG_IGN);
    signal(SIGSTOP,SIG_IGN);

    //2.变成独立的会话
    if(fork()>0)exit(0);
    setsid();
    //3.变成工作目录
    if(!cwd.empty())
    {
        chdir(cwd.c_str());
    }

    //重定向至/dev/null
    int fd = open(nullfile.c_str(), O_RDWR);
    if(fd >= 0)
    {
        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);
        close(fd);
    }
}