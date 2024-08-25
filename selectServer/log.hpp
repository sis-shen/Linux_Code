#pragma once

#include <iostream>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define LOG_FILE "log.txt"//日志文件名

#define SIZE 1024  //缓冲区大小

enum OUT_MODE//输出模式
{
    Screen,  //输出到显示器
    OneFile, //输出到一个文件
    MultiFile//按等级输出到多个文件
};

enum LEVEL //日志等级
{
    Info,    //一般信息
    Debug,   //debug日志
    Warning, //警告
    Error,   //错误
    Fatal    //失败
};

class Log
{
public:
    Log(OUT_MODE om = Screen):_om(om)//默认构造函数
    {}
    ~Log(){}//没有特殊用处
    void enable(OUT_MODE om)//启用指定的输出模式
    {
        _om = om;
    }
    std::string levelToString(LEVEL level)
    {
        switch (level)
        {
            case Info:return "Info";
            case Debug:return "Debug";
            case Warning:return "Warning";
            case Error:return "Error";
            case Fatal:return "Fatal";
        default: return "None";
        }
    }

    void logmessage(LEVEL level,const char *format,...)
    {
        char leftBuffer[SIZE] = {0};
        time_t t = time(nullptr);
        struct tm *ctime = localtime(&t);//获取时间

        snprintf(leftBuffer,sizeof(leftBuffer),"[%s][%d-%d-%d %d:%d:%d]",levelToString(level).c_str(),1900+ctime->tm_year,1+ctime->tm_mon,ctime->tm_mday,
            ctime->tm_hour,ctime->tm_min,ctime->tm_sec);

        va_list s;
        va_start(s,format);
        char rightBuffer[SIZE];
        vsnprintf(rightBuffer,sizeof(rightBuffer),format,s);
        va_end(s);

        char logtxt[SIZE*2];
        snprintf(logtxt,sizeof(logtxt),"%s %s",leftBuffer,rightBuffer);
        printLog(level,logtxt);;//打印日志
    }

    void operator()(LEVEL level,const char *format,...)
    {
        char leftBuffer[SIZE] = {0};
        time_t t = time(nullptr);
        struct tm *ctime = localtime(&t);//获取时间

        snprintf(leftBuffer,sizeof(leftBuffer),"[%s][%d-%d-%d %d:%d:%d]",levelToString(level).c_str(),1900+ctime->tm_year,1+ctime->tm_mon,ctime->tm_mday,
            ctime->tm_hour,ctime->tm_min,ctime->tm_sec);

        va_list s;
        va_start(s,format);
        char rightBuffer[SIZE];
        vsnprintf(rightBuffer,sizeof(rightBuffer),format,s);
        va_end(s);

        char logtxt[SIZE*2];
        snprintf(logtxt,sizeof(logtxt),"%s %s",leftBuffer,rightBuffer);
        printLog(level,logtxt);;//打印日志
    }

    void printLog(LEVEL level, const std::string& logtxt)
    {
        switch(_om)
        {
            case Screen:std::cout<<logtxt<<std::endl;break;
            case OneFile:printOneFile(LOG_FILE,logtxt);break;
            case MultiFile:printMultiFile(level,logtxt);break;
            default: break;
        }
    }

    void printOneFile(const std::string& logname,const std::string& logtxt)//传两个参数用于代码复用
    {
        int fd = open(logname.c_str(), O_WRONLY|O_CREAT|O_APPEND,0666);
        if(fd<0) return;
        write(fd,logtxt.c_str(),logtxt.size());
        close(fd);
    }

    void printMultiFile(LEVEL level,const std::string logtxt)
    {
        std::string filename = LOG_FILE;//构建文件名
        filename+= ".";
        filename += levelToString(level);
        printOneFile(filename,logtxt);//复用代码
    }
private:
    OUT_MODE _om = Screen;
};

Log log;
