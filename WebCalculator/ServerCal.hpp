#pragma once
#include <iostream>
#include "Protocol.hpp"


enum{
    DIV_ZERO = 1,
    MODE_ZERO,
    OTHER_OPER,
};

class ServerCal
{
public:
    ServerCal()
    {}
    Response CalculatorHelper(Request &req)
    {
        Response resp;
        switch(req.op)
        {
            case '+':resp.result = req.x + req.y;break;
            case '-':resp.result = req.x - req.y;break;
            case '*':resp.result = req.x * req.y;break;
            case '/':
                {
                    if(req.y == 0)
                    {
                        resp.code = DIV_ZERO;
                    }
                    else 
                    {
                        resp.result = req.x / req.y;
                    }
                }
                break;
            case '%':
                {
                    if(req.y == 0)
                    {
                        resp.code = MODE_ZERO;
                    }
                    else 
                    {
                        resp.result = req.x / req.y;
                    }
                }
                break;
            default:
                resp.code = OTHER_OPER;
                break;
        }

    }
    std::string Calculator(std::string&package)
    {
        std::string content;
        bool r = Decode(package,&content);
        if( !r )return "";//报文不完整
        Request req;
        r = req.Deserialize(content);
        if(!r)return "";

        content  = "";
        Response resp = CalculatorHelper(req);
        resp.Serialize(&content);
        content = Encode(content);
        return content;
    }

};