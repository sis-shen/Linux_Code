#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>

int main()
{
    Json::Value root;

    root["x"] = 100;
    root["y"] = 22565;
    root["op"] = '+';
    root["dict"] = "this is a dict";

    // Json::FastWriter fw;
    Json::StyledWriter w;
    std::string ret = w.write(root);

    std::cout<<ret<<std::endl;

    Json::Value v;
    Json::Reader reader;
    reader.parse(ret,v);

    int x = v["x"].asInt();
    int y = v["x"].asInt();
    char op = v["op"].asInt();
    std::string str = v["dict"].asString();

    std::cout<<x<<std::endl;
    std::cout<<y<<std::endl;
    std::cout<<op<<std::endl;
    std::cout<<str<<std::endl;
    
    
    return 0;
}