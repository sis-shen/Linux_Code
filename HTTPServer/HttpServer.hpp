#pragma once

#include "log.hpp"
#include "Socket.hpp"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>


const std::string wwwroot = "./wwwroot"; // web根目录
const std::string sep = "\r\n";
const std::string homepage = "/index.html";//主页文件

const uint16_t default_port = 25565;//HTTP通信使用的端口

class HttpServer;//提前声明

struct ThreadData
{
    ThreadData(int fd, HttpServer *svr)
        : sockfd(fd), httpsvr(svr)
    {
    }
    int sockfd;
    HttpServer *httpsvr;
};

class HttpRequest
{
public:
    void Deserialize(std::string req)
    {
        while (true)
        {
            //逐行分解首部
            std::size_t pos = req.find(sep);
            if (pos == std::string::npos)
                break;
            std::string temp = req.substr(0, pos);
            if (pos == 0)
                break;
            req_header.push_back(temp);
            req.erase(0, pos + sep.size());
        }
        //将剩下的Body存入text
        text = req;
    }

    void Parse()
    {
        std::stringstream ss(req_header[0]);//获取首部的首行
        ss >> _method >> _url >> _http_version;
        _res_path = wwwroot;//获取根目录
        if(_url == "/" || _url == "index.html")_res_path+= homepage;
        else _res_path += _url;//非首页的处理方式

        auto pos = _res_path.find(".");
        if(pos == std::string::npos) _suffix = ".html";//自动添加文件后缀
        else _suffix = _res_path.substr(pos);
    }

    void DebugPrint()
    {
        //逐行打印首部
        std::cout << "\n----------------------------\n";
        for (auto &line : req_header)
        {
            std::cout << line << "\n\n";
        }
        std::cout << "----------------------------\n";
        std::cout<<text<<std::endl;//打印Body

        //打印获取到的信息
        std::cout<<"methond: "<<_method<<std::endl;
        std::cout<<"url: "<<_url<<std::endl;
        std::cout<<"http version: "<<_http_version<<std::endl;
        std::cout<<"resource path: "<<_res_path<<std::endl;
    }

public:
    std::vector<std::string> req_header;
    std::string text;

    // 解析结果
    std::string _method;
    std::string _url;
    std::string _http_version;
    std::string _res_path;

    std::string _suffix;
};

class HttpServer
{
public:
    HttpServer(uint16_t port = default_port)
        : _port(port)
    {
        _content_type.insert({".html","text/html"});
        _content_type.insert({".png","image/html"});
        _content_type.insert({".jpg","image/jpeg"});
        
    }

    ~HttpServer() {}

    bool Start()
    {
        _listen_sock.Socket();
        _listen_sock.Bind(_port);
        _listen_sock.Listen();
        log(Info, "Server started running");
        for (;;)
        {
            std::string clientip;
            uint16_t clientport;
            int sockfd = _listen_sock.Accept(&clientip, &clientport);
            std::cout<<"\n";
            log(Info, "get a new link [%s:%u] sockfd:%d", clientip.c_str(), clientport, sockfd);
            pthread_t tid;
            ThreadData *td = new ThreadData(sockfd, this);
            pthread_create(&tid, nullptr, ThreadRun, td);
        }
    }

    static void *ThreadRun(void *args)
    {
        pthread_detach(pthread_self());
        // log(Info,"create a thread");
        ThreadData *td = static_cast<ThreadData *>(args);
        HandlerHttp(td->sockfd, td->httpsvr);
        delete td;
        return nullptr;
    }

    static std::string ReadHtmlContent(const std::string &htmlpath)
    {
        std::ifstream in(htmlpath,std::ios::binary);
        if (!in.is_open())
            return "";

        in.seekg(0,std::ios::end);
        auto len = in.tellg();
        in.seekg(0,std::ios::beg);

        std::string content;
        content.resize(len);
        in.read((char*)content.c_str(),content.size());

        in.close();
        return content;
    }

    std::string Suffix2Desc(const std::string& suffix)
    {
        auto iter = _content_type.find(suffix);
        if(iter == _content_type.end()) return _content_type[".html"];
        else return _content_type[suffix];
    }

    static void HandlerHttp(int sockfd, HttpServer *httpsvr)
    {
        char buffer[10240];
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        // log(Info,"get a msg, n = %d",n);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer; // 先假设读取到一个完整的，独立的http请求
            HttpRequest req;
            req.Deserialize(buffer);
            req.Parse();
            // req.DebugPrint();

            std::string path = req._res_path;

            // 返回相应的过程
            bool ok = true;
            std::string text = ReadHtmlContent(path);

            if(text.empty())
            {
                std::string err_html = wwwroot;
                err_html+="/";
                err_html+="err404.html";
                text = ReadHtmlContent(err_html);
                ok=false;
            }

            std::string response_line;
            if(ok)
                response_line = "HTTP/1.0 200 OK\r\n";
            else 
                response_line = "HTTP/1.0 404 Bad\r\n";
            
            //response_line = "HTTP/1.0 302 Bad\r\n";
            
            std::string response_header = "Content-Length: ";
            
            response_header += std::to_string(text.size());
            response_header += "\r\n";
            response_header += httpsvr->Suffix2Desc(req._suffix);
            response_header += "\r\n";
            response_header+="Set-Cookie: name=supdriver";
            response_header += "\r\n";
            response_header+="Set-Cookie: password=password";
            response_header += "\r\n";
            response_header+="Set-Cookie: view=./a/b/hello.html";
            response_header += "\r\n";            
            //response_header += "Location: https://www.supdriver.top/\r\n"; //重定向
            std::string block_line = "\r\n";

            std::string response = response_line;
            response += response_header;
            response += block_line;
            response += text;

            send(sockfd, response.c_str(), response.size(), 0);
        }
        close(sockfd);
    }

private:
    Sock _listen_sock;
    uint16_t _port;
    std::unordered_map<std::string,std::string> _content_type;
};