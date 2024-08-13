#include "task.hpp"
#include <ctime>
#include <sys/stat.h>
#include <sys/wait.h>


const int processNum = 5;//最大进程数
std::vector<task_t> tasks;//承载任务的列表

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
        int cmdcode = 0;
        int n = read(0,&cmdcode,sizeof(int));//阻塞等待读取任务码
        if(n == sizeof(int))//成功获取任务码
        {
            //std::cout<<"child say@ "<<"get cmdcode: "<<cmdcode<<std::endl;//DEBUG
            if(cmdcode >=0 && cmdcode < tasks.size())
            {
                tasks[cmdcode]();
            }
            else 
            {
                std::cout<<"wrong cmdcode: "<<cmdcode 
                    <<" max size: "<<tasks.size()<<std::endl;
            }
        }
        else if( n == 0) break;//读端关闭，退出slaver
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
            dup2(pipefd[0],0);//子进程输入重定向
            slaver();
            std::cout<<"proccess "<< getpid() << " quit"<<std::endl;
            exit(0);
        } 
        //father
        close(pipefd[0]);
        std::string name = "process-"+std::to_string(i);
        channels->push_back(channel(pipefd[1],id,name));
    }
}

void Debug(const std::vector<channel> channels)
{
    for(auto &c:channels)
    {
        std::cout<<c._cmdfd << " " << c._slaverid<< " " << c._processname << std::endl;
    }
}

void ctrlSlaver(const std::vector<channel> &channels)
{
    for(int i = 0;i<10;i++)//随机派发10个任务
    {
        int cmdcode = rand() % tasks.size();//获取随机任务码
        int select_num = rand()%channels.size();//获取选择码
        std::cout<<"father say# "<<"taskcode: "<<cmdcode<<" send to "<<channels[select_num]._processname
            << std::endl;//输出日志
        write(channels[select_num]._cmdfd,&cmdcode,sizeof(cmdcode));//写入任务码，派发任务
        sleep(1);
    }
}

void QuitProcess(const std::vector<channel>& channels)
{
    //因为创建子进程时管道文件的连接没有进一步处理，关系复杂，所以下面两个循环一定要分开来
    for(const auto &c:channels)close(c._cmdfd);//关闭主进程管道文件
    for(const auto &c:channels)waitpid(c._slaverid,nullptr,0);//阻塞等待子进程，回收僵尸进程
}


int main()
{
    srand(time(nullptr) ^ getpid()^1023);//种随机数种子
    std::vector<channel> channels;
    //1.初始化
    LoadTask(&tasks);//必须先载入任务再产生子进程，否则子进程看不到任务列表
    InitChannels(&channels);

    //测试函数
    //Debug(channels);

    //2.控制子进程
    ctrlSlaver(channels);

    //3.关闭进程池
    QuitProcess(channels);

    return 0;
}