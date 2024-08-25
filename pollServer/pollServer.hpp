#pragma once

#include <fcntl.h>
#include <iostream>
#include <sys/poll.h>
#include "Socket.hpp"
#include "log.hpp"
#include <sys/time.h>
#include <string>

using std::cerr;
using std::cout;
using std::endl;

extern Log log;

static const uint16_t defaultport = 25565;
static const uint16_t fd_num_max = 64;
const int defaultfd = -1;
const int non_evnt = 0;

class PollServer
{
public:
    PollServer(uint16_t port = defaultport)
        : _port(port)
    {
        for (int i = 0; i < fd_num_max; ++i)
        {
            _event_fds[i].fd = defaultfd;
            _event_fds[i].events = non_evnt;
            _event_fds[i].revents = non_evnt;
        }
    }

    ~PollServer() { _listensock.Close(); }

    bool Init()
    {
        _listensock.Socket();
        _listensock.Bind(_port);
        _listensock.Listen();
        return true;
    }

    void Dispatcher()
    {
        for (int i = 0; i < fd_num_max; ++i)
        {
            int fd = _event_fds[i].fd;
            if (fd == defaultfd)
            {
                continue;
            }
            else if (_event_fds[i].revents & POLLIN)
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
            if (_event_fds[i].fd == defaultfd)
            {
                pos = i;
                break;
            }
        }
        if (pos < fd_num_max)
        {
            _event_fds[pos].fd = sock;
            _event_fds[pos].events = POLLIN; // 监听读事件
            _event_fds[pos].revents = non_evnt;
        }
        else
        {
            // 丢弃或扩容
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
            _event_fds[i].fd = defaultfd;
            _event_fds[i].events = non_evnt;
        }
        else
        {
            log(Warning, "read error ,fd:%d", fd);
            close(fd);
            _event_fds[i].fd = defaultfd;
            _event_fds[i].events = non_evnt;
        }
    }

    void Start()
    {

        int listenfd = _listensock.Fd();

        _event_fds[0].fd = listenfd;
        _event_fds[0].events = POLLIN;

        int timeout = 3000; // ms

        for (;;)
        {

            int n = poll(_event_fds, fd_num_max, 3000);
            switch (n)
            {
            case 0:
                cout << "time out... " << endl;
                break;
            case -1:
                cerr << "poll err" << endl;
                break;

            default:
                log(Info, "get a new link");
                Dispatcher();
                break;
            }
        }
    }

private:
    Sock _listensock;
    uint16_t _port;
    struct pollfd _event_fds[fd_num_max];
};