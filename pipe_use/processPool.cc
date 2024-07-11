#include "task.hpp"
#include <string>
#include <ctime>

const int processNum = 5;//最大进程数

//先描述
class channel
{
public:
    channel(int cmdfd,pid_t slaverid,std::string& name)
    :_cmdfd(cmdfd),_slaverid(slaverid),_processname(name)
    {}

public:
    int _cmdfd;                    //发送任务的文件描述符
    pid_t _slaverid;               //子进程的id 
    std::string _processname;             //子进程的名字  --方便打印日志
};


void slaver()
{
    while(true)
    {
        sleep(1);
    }
}

//形参命名约定
//输入: const &
//输出: *
//输入输出: &

void InitChannels(std::vector<channel>* channels)
{
    for(int i = 0;i<processNum;i++)
    {
        int pipefd[2];
        int n = pipe(pipefd);
        assert(!n);
        (void)n;//调用一下n

        pid_t id = fork();
        if(id == 0)//child
        {
            close(pipefd[1]);
            dup2(pipefd[0],0);//输入重定向
            slaver();
            //slaver(pipefd[0]);
            exit(0);
        } 
        //father
        close(pipefd[0]);
        std::string name = "process-"+std::to_string(i);
        channels->push_back(channel(pipefd[1],id,name));
    }
}


int main()
{
    srand(time(nullptr) ^ getpid()^1023);//种随机数种子
    std::vector<channel> channels;
    //初始化
    InitChannels(&channels);

    for(auto &c:channels)
    {
        std::cout<<c._cmdfd << " " << c._slaverid<< " " << c._processname << std::endl;
    }
    return 0;
}