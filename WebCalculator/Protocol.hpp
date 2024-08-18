#pragma once

#include <iostream>
#include <string>

const std::string blank_space_sep = " ";
const std::string protocal_sep = "\n";

std::string Encode(std::string &content)
{
    std::string package = std::to_string(content.size());
    package += protocal_sep;
    package += content;
    package+=protocal_sep;
    return package;
}

bool Decode(std::string& package,std::string *out)
{
    size_t pos = package.find(protocal_sep);
    if(pos == std::string::npos) return false;
    std::string len_str = package.substr(0,pos);
    size_t len = std::stoi(len_str);

    std::size_t total_len = len_str.size() + len + 2*protocal_sep.size();
    if(package.size() < total_len) return false;

    *out = package.substr(pos+1,len);

    //移除报文
    package.erase(0,total_len);

    return true;
}

class Request
{
public:
    Request(int data1 = 0, int data2 = 0, char oper = '+')
        : x(data1), y(data2), op(oper)
    {
    }

public:
    bool Serialize(std::string *out)
    {
        // struct =>string x op y
        // 有效载荷
        std::string s = std::to_string(x);
        s += blank_space_sep;
        s += op;
        s += blank_space_sep;
        s += std::to_string(y);

        // 封装报头
        *out = s;
        return true;
    }

    bool Deserialize(const std::string &in)
    {
        size_t left_pos = in.find(blank_space_sep);
        if(left_pos == std::string::npos) return false;
        std::string part_x = in.substr(0,left_pos);

        size_t right_pos = in.find(blank_space_sep,left_pos+1);
        if(right_pos == std::string::npos) return false;
        std::string part_y = in.substr(right_pos+1);

        if(left_pos+1 != right_pos-1)return false;
        
        op = in[left_pos+1];
        x = std::stoi(part_x);
        y = std::stoi(part_y);
        return true;
    }

public:
    int x;
    int y;
    char op; // + - * / %
};

class Response
{
public:
    Response(int res = 0, int c = 0)
        : result(res), code(c)
    {
    }

    bool Serialize(std::string *out)
    {
        std::string s = std::to_string(result);
        s += blank_space_sep;
        s += std::to_string(code);

        // 封装报头
       *out =s;
        return true;
    }

    bool Deserialize(const std::string &in)
    {
        size_t pos = in.find(blank_space_sep);
        if(pos == std::string::npos) return false;
        std::string part_left = in.substr(0,pos);
        std::string part_right = in.substr(pos+1);

        result = std::stoi(part_left);
        code = std::stoi(part_right);
        return true;
    }

public:
    int result;
    int code; // 0为可信，非零为错误码
};