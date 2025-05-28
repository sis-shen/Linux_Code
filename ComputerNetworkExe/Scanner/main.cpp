#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <iostream>

using namespace std;

// 设置socket为非阻塞
int set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

// 检查TCP端口是否开放
int check_tcp_port(const char *ip, int port, int timeout_sec) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 0;
    }

    // 设置非阻塞
    set_nonblocking(sockfd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    fd_set wfds;
    struct timeval tv;
    FD_ZERO(&wfds);
    FD_SET(sockfd, &wfds);
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    //select监听端口响应
    int ret = select(sockfd + 1, NULL, &wfds, NULL, &tv);
    if (ret > 0) {
        int err;
        socklen_t len = sizeof(err);
        getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len);
        close(sockfd);
        if (err == 0) {
            cout<<"tcp端口:"<<port<<"开放\n";
            return 1; // 端口开放
        }
    }

    close(sockfd);
    return 0; // 端口关闭或超时
}

// 检查UDP端口是否开放
int check_udp_port(const char *ip, int port, int timeout_sec) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 0;
    }

    // 设置非阻塞
    set_nonblocking(sockfd);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    // 发送空数据包
    sendto(sockfd, NULL, 0, 0, (struct sockaddr*)&addr, sizeof(addr));

    fd_set readfds;
    struct timeval tv;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;

    int ret = select(sockfd + 1, &readfds, NULL, NULL, &tv);
    if (ret > 0) {
        cout<<"udp端口:"<<port<<"开放\n";
        close(sockfd);
        return 1;
    }

    close(sockfd);
    cout<<"udp端口:"<<port<<"无响应\n";
    return 0; // 无响应，可能关闭
}
int main(int argc,char* args[]) {
    if(argc != 2)
    {
        std::cout<<"usage: "<<args[0] <<" <ip> \n";
        return 0;
    }
    std::string target_ip = args[1];  // 目标 IP 地址

    uint16_t tcp_cnt = 0;
    uint16_t udp_cnt = 0;

    std::cout<< "开始扫描TCP端口"<<std::endl;
    for(int i =1;i<UINT16_MAX;++i)
    {
        if(check_tcp_port(target_ip.c_str(),i,1)) tcp_cnt++;
    }
    std::cout<< "TCP端口扫描完毕,端口可用数 "<< tcp_cnt << "/" << UINT16_MAX<<std::endl;

    std::cout<< "开始扫描UDP端口"<<std::endl;
    for(int i =1;i<UINT16_MAX;++i)
    {
        if(check_udp_port(target_ip.c_str(),i,1)) udp_cnt++;
    }
    std::cout<< "UDP端口扫描完毕,端口可用数 "<< udp_cnt << "/" << UINT16_MAX<<std::endl;

    return 0;
}
