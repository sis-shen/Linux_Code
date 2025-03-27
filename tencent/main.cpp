#include "httplib.h"
#include <jsoncpp/json/json.h>
#include <iostream>
#include <memory>

enum ERRORCODE
{
    LINK_ERROR = 1,
    STATUS_ERROR,
    JSON_MIS_KEY,
    JSON_WRONG_TYPE
};

class Response
{
public:
    void parse(const std::string& jsonStr)
    {
        Json::Value root;
        Json::CharReaderBuilder reader;
        std::unique_ptr<Json::CharReader> jsonReader(reader.newCharReader());
        if(!jsonReader->parse(jsonStr.data(),jsonStr.data()+jsonStr.size(),&root,nullptr))
        {
            std::cout<<"反序列化失败\n";
            exit(-1);
        }
        //headers
        if(!(root.isMember("headers") ))
        {
            std::cout<<"headers字段不存在！\n";
            exit(JSON_MIS_KEY);
        }
        if(!root["headers"].isArray())
        {
            std::cout<<"headers类型错误\n";
            // exit(JSON_WRONG_TYPE);
        }

        Json::Value headersObj = root["headers"];
        if(!(headersObj.isMember("Host") ))
        {
            std::cout<<"Host字段不存在！\n";
            exit(JSON_MIS_KEY);
        }
        if(!headersObj["Host"].isString())
        {
            std::cout<<"Host类型错误\n";
            exit(JSON_WRONG_TYPE);
        }
        _headers._host = headersObj["Host"].asString();
    

        //oringin
        if(!(root.isMember("origin") ))
        {
            std::cout<<"origin字段不存在！\n";
            exit(JSON_MIS_KEY);
        }
        if(!root["origin"].isString())
        {
            std::cout<<"origin类型错误\n";
            exit(JSON_WRONG_TYPE);
        }
        _origin=root["origin"].asString();

        //url

        if(!(root.isMember("url") ))
        {
            std::cout<<"url字段不存在！\n";
            exit(JSON_MIS_KEY);
        }
        if(!root["url"].isString())
        {
            std::cout<<"url类型错误\n";
            exit(JSON_WRONG_TYPE);
        }
        _url=root["url"].asString();

    }

    void debugPrint()
    {
        std::cout<<"orgin:"<<_origin<<std::endl;
        std::cout<<"url:"<<_url<<std::endl;
        std::cout<<"host:"<<_headers._host<<std::endl;
    }

public:
    struct Headers
    {
        std::string _host;
    };

    std::string _origin;
    std::string _url;
    Headers _headers;
};

using std::cout;
int main()
{

// HTTP
    httplib::Client cli("http://httpbin/-*/ .org");

    auto res = cli.Get("/get");

    if(res->status != 200)
    {
        cout<<"请求出错，状态码:"<<res->status<<std::endl;
        exit(LINK_ERROR);
    }
    // std::cout<<res->body;


    Response myRes;
    myRes.parse(res->body);
    myRes.debugPrint();
    return 0;
}