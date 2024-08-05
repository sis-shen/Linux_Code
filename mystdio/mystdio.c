#include "Mystdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define FILE_MODE 0666 //设置默认的文件权限

_FILE * _fopen(const char *filename,const char *flag)
{
    int f = 0;//准备空位图
    int fd = -1;
    if(strcmp(flag,"w") == 0)
    {
        f = (O_CREAT|O_WRONLY|O_TRUNC);
        fd = open(filename,f,FILE_MODE);//打开文件
    }
    else if(strcmp(flag,"a") == 0)
    {
        f = (O_CREAT|O_WRONLY|O_APPEND);
        fd = open(filename,f,FILE_MODE);//打开文件
    }
    else if(strcmp(flag,"w") == 0)
    {
        f = O_RDONLY;
        fd = open(filename,f);//打开文件
    }
    else return NULL;//非法的打开模式

    if(fd == -1) return NULL;//打开失败
    
    _FILE *fp = (_FILE*)malloc(sizeof(_FILE));//申请资源
    fp->fileno = fd;//储存文件描述符
    fp->flag = FLUSH_LINE;//设置为行刷新
    fp->in_pos = 0;//指针置0
    fp->out_pos = 0;//指针置0
    return fp;
}

int _fwrite(_FILE *fp,const char* s,int len)
{
    memcpy(&(fp->outbuffer[fp->out_pos]),s,len);//将内容拷贝至缓冲区指定位置
    fp->out_pos +=len;//简易偏移out_pos
    if(fp->fileno == FLUSH_NONE)
    {
        wrtie(fp->fileno,s,fp->out_pos);//无缓冲区的写入版本
    }
    else if(fp->flag == FLUSH_LINE)
    {
        if(fp->outbuffer[fp->out_pos-1] == '\n')
        {
            wrtie(fp->fileno,s,fp->out_pos);//立即刷新
            fp->out_pos = 0;
        }
        else{
            return len;
        }
    }
    else if(fp->flag == FLUSH_ALL)
    {
        if(fp->out_pos == SIZE)
        {
            wrtie(fp->fileno,s,fp->out_pos);//立即刷新
            fp->out_pos = 0;
        }
        else{
            return len;
        }
    }
}

void _fclose(_FILE* fp)
{
    if(fp == NULL) return;//防止空指针
    close(fp->fileno);//关闭文件
    free(fp);//释放资源
}