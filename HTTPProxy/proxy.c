#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "proxy_parse.h"

#define BUFSIZE 8192

void error(const char *msg) { perror(msg); exit(1); }

int create_listen_socket(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("socket() failed");
    
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("bind() failed");
    
    listen(sockfd, 5);
    return sockfd;
}

int connect_target(const char *host, int port) {
    struct hostent *server = gethostbyname(host);
    if (!server) error("DNS resolve failed");
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("socket() failed");
    
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(port);
    
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("connect() failed");
    
    return sockfd;
}

void forward_data(int from_fd, int to_fd) {
    char buffer[BUFSIZE];
    ssize_t n;
    while ((n = recv(from_fd, buffer, BUFSIZE, 0)) > 0) {
        if (send(to_fd, buffer, n, 0) < 0) break;
    }
}

void process_request(int client_fd) {
    char request_buf[BUFSIZE];
    ssize_t bytes_read = recv(client_fd, request_buf, BUFSIZE-1, 0);
    if (bytes_read <= 0) return;
    request_buf[bytes_read] = '\0';

    // 解析请求
    struct ParsedRequest *req = ParsedRequest_create();
    if (ParsedRequest_parse(req, request_buf, bytes_read) < 0) {
        const char *bad_req = "HTTP/1.0 400 Bad Request\r\n\r\n";
        send(client_fd, bad_req, strlen(bad_req), 0);
        ParsedRequest_destroy(req);
        return;
    }

    // 获取目标信息
    int port = req->port ? atoi(req->port) : 80;
    if (port <= 0) port = 80;

    // 连接目标服务器
    int target_fd = connect_target(req->host, port);
    
    // 修正Host头
    char host_header[256];
    snprintf(host_header, sizeof(host_header), "%s:%d", req->host, port);
    ParsedHeader_set(req, "Host", host_header);

    // 构造转发请求
    size_t req_len = ParsedRequest_totalLen(req);
    char *forward_req =(char *) malloc(req_len);
    ParsedRequest_unparse(req, forward_req, req_len);
    
    // 转发请求
    send(target_fd, forward_req, req_len, 0);
    free(forward_req);
    
    // 双向转发数据
    forward_data(target_fd, client_fd); // 响应
    forward_data(client_fd, target_fd); // 其他请求数据(如有)
    
    close(target_fd);
    ParsedRequest_destroy(req);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    
    int listen_fd = create_listen_socket(atoi(argv[1]));
    while (1) {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0) continue;
        
        process_request(client_fd);
        close(client_fd);
    }
    return 0;
}