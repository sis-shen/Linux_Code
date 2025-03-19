#include "logger.hpp"
#include <string>
#include <string.h>
#include "httplib.h"
const std::string log_file("log_file.txt");
const int32_t level = spdlog::level::warn;
//这次是DEBUG模式
const bool release_mode = false;

void test1()
{
    double j = 2.0;
    for(int i = 0;i<100000000;++i)
    {
        j += 1.0+2.111;
    }
}

int main() 
{ 
    btyGoose::init_logger(release_mode,log_file,level);
    httplib::Server svr;
    svr.Post("/consumer/dish/dishInfo", [](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        test1();
        res.set_header("Content-Type", "application/json;charset=UTF-8");
        res.body = "{\"body\":666666}";
        LOG_TRACE("{}",res.body);
    });
    svr.Get("/ping", [](const httplib::Request& req, httplib::Response& res) {
        LOG_DEBUG("收到HTTP请求, method={},path={}",req.method,req.path);
        res.body = "pong";
        LOG_TRACE("{}",res.body);
    });
    svr.listen("0.0.0.0",8501);
    return 0; 
}