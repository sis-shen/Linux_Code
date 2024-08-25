#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>
#include "log.hpp"
#include "Socket.hpp"
#include "Epoller.hpp"

static const uint16_t defaultport = 25565;

class Connection;
class TcpServer;

using func_t = std::function<void(std::shared_ptr<Connection>)>;

class Connection
{
private:
    int _sock;
    std::string inbuffer;//针对字节流的缓冲区
    std::string outbuffer;//针对字节流的缓冲区

    func_t _recv_cb;
    func_t _send_cb;
    func_t _except_ct;

    std::shared_ptr<TcpServer> _tcpserver_ptr;
};

class TcpServer: public nocopy
{
public:
    TcpServer(uint16_t port = defaultport)
    :_epoller_ptr(new Epoller()),_port(port),_listensocket_ptr(new Sock())
    {
        _listensocket_ptr->Socket();
        _listensocket_ptr->Bind(_port);
        _listensocket_ptr->Listen();
    }

    ~TcpServer(){}

    void Init()
    {

    }

    void Start()
    {
        
    }
    

private:
    std::shared_ptr<Sock> _listensocket_ptr;
    std::shared_ptr<Epoller> _epoller_ptr;
    std::unordered_map<int,std::shared_ptr<Connection> > _connections;//fd -> connection
    uint16_t _port;
};