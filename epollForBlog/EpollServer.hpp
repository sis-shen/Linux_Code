#include "Epoll.hpp"
#include <string>
#include <functional>

typedef std::function<void(const std::string& req,std::string&res)> Handler;

static Handler default_hander([](const std::string&req,std::string& res){
    res = req;//做一个回显处理
    return;
});

//这是一个只读Server
class EpollServer
{
public:
    EpollServer(uint16_t port = 80,Handler handler = default_hander):_port(port),_handler(handler){}

    void Start()
    {
        //创建监听套接字
        Sock listen_sock;
        listen_sock.Socket();
        listen_sock.Bind(_port);
        listen_sock.Listen(10);
        std::cout<<"服务器在端口:"<< _port << "开放!\n";
        Epoll epoll;//实例化一个epoll对象
        if(!epoll.Add(listen_sock)) return;
        //进入事件循环
        for(;;)
        {
            std::vector<Sock> output;
            if(!epoll.Wait(&output))
            {
                continue;
            }
            for(auto sock : output)
            {
                if(sock.Fd() == listen_sock.Fd())
                {
                    //监听套接字调用Accept
                    std::string client_ip;
                    uint16_t client_port;
                    int fd = listen_sock.Accept(&client_ip,&client_port);
                    epoll.Add(Sock(fd));
                    //因为对客户端具体信息不感兴趣，就丢弃了
                }
                else
                {
                    //如果是用于通信，则进行一次读写
                    std::string req,res;//request 和 response
                    int ret = sock.Recv(req);//获取request
                    if(ret <= 0)
                    {
                        //发生错误,或者关闭连接都要去除
                        epoll.Del(sock);
                        sock.Close();
                        continue;
                    }
                    std::cout<<"Server get msg@"<<req<<"\n";
                    _handler(req,res);//获取响应
                    ret = sock.Send(res);
                    if(ret < 0)
                    {
                        //发生错误,或者关闭连接都要去除
                        epoll.Del(sock);
                        sock.Close();
                        continue;
                    }
                }
            }
        }
    }

private:
    uint16_t _port;
    Handler _handler;
};