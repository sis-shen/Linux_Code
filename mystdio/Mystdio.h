#ifndef __MYSTDIO_H__
#define __MYSTDIO_H__

#include <string.h>

#define SIZE 1024 //定义缓冲区大小
#define FLUSH_NONE 0 //无缓冲
#define FLUSH_LINE 1 //行缓冲
#define FLUSH_ALL 2

typedef struct IO_FILE{
    int fileno;//文件描述符
    int flag; //刷新规则标志
    char inbuffer[SIZE];//输入缓冲区
    int in_pos;//输入缓冲区指针
    char outbuffer[SIZE];//输出缓冲区
    int out_pos;//输出缓冲区指针
}_FILE;

_FILE * _fopen(const char *filename,const char *flag);
int _fwrite(_FILE* fp,const char*s, int len);
void _fclose(_FILE* fp);

#endif 