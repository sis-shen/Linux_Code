#pragma once

#include <fcntl.h>
#include <iostream>
#include <sys/select.h>
#include "Socket.hpp"
#include <sys/time.h>
#include <string>

using std::cerr;
using std::cout;
using std::endl;

extern Log log;

static const uint16_t defaultport = 25565;
static const uint16_t fd_num_max = sizeof(fd_set) * 8;
const int defaultfd = -1;

class SelectServer
{
public:
    SelectServer(uint16_t port = defaultport)
        : _port(port)
    {
        for (int i = 0; i < fd_num_max; ++i)
        {
            _fd_array[i] = defaultfd;
        }
    }

    ~SelectServer() { _listensock.Close(); }

    bool Init()
    {
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
    }

    void Dispatcher(fd_set &rfds)
    {
        for (int i = 0; i < fd_num_max; ++i)
        {
            int fd = _fd_array[i];
            if (fd == defaultfd)
            {
                continue;
            }
            else if (FD_ISSET(fd, &rfds))
            {
                if (fd == _listensock.Fd())
                {
                    Acceptor();
                }
                else
                {
                    Recver(fd, i);
                }
            }
        }
    }

    void Acceptor()
    {
        // 链接事件就绪
        std::string clientip;
        uint16_t clientport = 0;
        int sock = _listensock.Accept(&clientip, &clientport);
        if (sock < 0)
            return;
        log(Info, "accept success, [%s:%d]", clientip.c_str(), clientport);

        int pos = fd_num_max;
        for (int i = 0; i < fd_num_max; ++i)
        {
            if (_fd_array[i] == defaultfd)
            {
                pos = i;
                break;
            }
        }
        if (pos < fd_num_max)
        {
            _fd_array[pos] = sock;
        }
        else
        {
            log(Warning, "连接数超载，已丢弃本次链接 fd: %d", sock);
            close(sock);
        }
    }

    void Recver(int fd, int i)
    {
        char buffer[1024] = {0};
        int n = read(fd, buffer, 1024 - 1);
        if (n > 0)
        {
            cout << "get a msg@ " << buffer << endl;
        }
        else if (n == 0)
        {
            log(Info, "client quit,close fd:%d", fd);
            close(fd);
            _fd_array[i] = defaultfd;
        }
        else
        {
            log(Warning, "read error ,fd:%d", fd);
            close(fd);
            _fd_array[i] = defaultfd;
        }
    }

    void Start()
    {

        int listenfd = _listensock.Fd();
        _fd_array[0] = _listensock.Fd();

        for (;;)
        {
            fd_set rfds;
            FD_ZERO(&rfds);
            struct timeval timeout = {1, 0};

            int maxfd = _fd_array[0];
            for (int i = 0; i < fd_num_max; ++i)
            {
                if (_fd_array[i] == defaultfd)
                    continue;
                ;
                FD_SET(_fd_array[i], &rfds);
                if (maxfd < _fd_array[i])
                {
                    maxfd = _fd_array[i];
                }
            }

            int n = select(maxfd + 1, &rfds, nullptr, nullptr, &timeout);
            switch (n)
            {
            case 0:
                cout << "time out timeout:" << timeout.tv_sec << "." << timeout.tv_usec << endl;
                break;
            case -1:
                cerr << "select err" << endl;
                break;

            default:
                log(Info, "get a new link");
                Dispatcher(rfds);
                break;
            }
        }
    }

private:
    Sock _listensock;
    uint16_t _port;
    int _fd_array[fd_num_max];
};