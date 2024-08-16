#pragma once

#include <iostream>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_NAME "./myfifo" //设置管道文件名
#define MODE 0664 //设置管道文件的权限

enum//枚举错误码
{
    FIFO_CREAT_ERR = 1,
    FIFO_DELETE_ERR,
    FIFO_OPEN_ERR
};