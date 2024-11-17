#include "TCPSocket.hpp"
#include <unordered_set>
#include <vector>
#include <sys/wait.h>
#include <sys/signal.h>

class TCPEchoServer
{
public:
    TCPEchoServer(const std::string& ip,const uint16_t port)
    :_ip(ip),_port(port){}

    void run()
    {
        if(_socket.Socket() == false) return;
        if(_socket.Bind(_ip,_port) == false) return;
        if(_socket.Listen() == false)return;

        while(true)
        {
            std::string client_ip;
            uint16_t client_port;
            printf("waiting for acception\n");
            int client_fd = _socket.Accept(&client_ip,&client_port);

            if(client_fd < 0)
            {
                //本次accept失败，等待1s后再次尝试
                sleep(1);
                continue;
            }
            //accept成功
            broadcast(client_ip,client_port);
            _clients.insert(client_fd);

            pid_t pid = fork();
            if(pid == 0)
            {
                _socket.Close();//关闭文件描述符
                if(fork() != 0) exit(0);//子进程退出
                //孙子进程成为孤儿进程
                child_loop(client_fd);//执行工作循环
                close(client_fd);
                exit(0);
            }
            else
            {
                pid_t rid = waitpid(pid,nullptr,0);
                (void) rid;//调用一下rid，防止报警告
            }

        }
    }

private:
    void broadcast(const std::string& ip,const uint16_t port)
    {
        std::string who;
        who+="[";
        who+=ip;
        who+=":";
        who+=std::to_string(port);
        who+="]";

        std::string msg(who);
        msg+="Entered Server\n";

        std::vector<int> del_list;//后置删除断开连接的文件描述符
        std::cout<<msg<<std::endl;
        for(auto fd:_clients)
        {
            int ret = write(fd,msg.c_str(),msg.size());
            if(ret == 0)
            {
                del_list.push_back(fd);
            }
        }
        std::cout<<std::endl;

        for(auto fd:del_list)
        {
            close(fd);//关闭文件描述符
            _clients.erase(fd);//去除列表
        }
    }

    //子进程提供回显服务的函数入口
    void child_loop(int fd)
    {
        while(true)
        {
            char buffer[128];
            int n =read(fd,buffer,sizeof(buffer)-1);//读取信息
            if(n<=0)return;//读取异常或连接关闭，退出工作函数
            buffer[n] = '\0';
            
            n = write(fd,buffer,sizeof(buffer)-1);
            if(n<=0)return;//输出异常或连接关闭，退出工作函数
        }
    }

public:
    ~TCPEchoServer()
    {
        _socket.Close();
        for(auto fd:_clients)//关闭所有通信用文件描述符
        {
            close(fd);
        }
    }

private:
    std::string _ip;
    uint16_t _port;
    TCPSocket _socket;
    std::unordered_set<int> _clients;
};