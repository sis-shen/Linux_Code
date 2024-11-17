#pragma once
#include <jsoncpp/json/json.h>
#include <string>
#include <fstream>
#include <sstream>

class DBUserConfig
{
public:
    DBUserConfig(const std::string &file = "./userConfig",
                 const std::string &user = "root",
                 const std::string &password = "",
                 const std::string &database = "",
                 const std::string &ip = "127.0.0.1",
                 size_t port = 3306)
        : _file(file), _user(user),_password(password), _database(database), _ip(ip), _port(port)
    {
    }

    //实现setter
    void setFile(const std::string &file) { _file = file; }
    void setUser(const std::string &user) { _user = user; }
    void setPassword(const std::string & password){ _password = password; }
    void setDatabase(const std::string &database) { _database = database; }
    void setIp(const std::string &ip) { _ip = ip; }
    void setPort(size_t port) { _port = port; }

    std::string Address()
    {
        std::string ret = _ip;
        ret += ":";
        ret += std::to_string(_port);
        return ret;
    }
    //实现getter
    std::string User() { return _user; }
    std::string Password() {return _password; }
    std::string DataBase() { return _database; }
    std::string Ip() { return _ip; }
    size_t Port() { return _port; }

    //实现基于文件的持久化
    void writeFile()
    {
        Json::Value root;

        root["user"] = _user;
        root["database"] = _database;
        root["ip"] = _ip;
        root["port"] = _port;
        root["password"] = _password;

        Json::StyledWriter w;
        std::ofstream ofs(_file,std::ios::out);

        ofs<< w.write(root);
        ofs.close();
    }

    bool readFile()
    {
        std::ifstream ifs(_file,std::ios::in);
        if(!ifs.is_open())return false;//打开文件
        std::string str;
        std::stringstream buf;
        buf<<ifs.rdbuf();
        str = buf.str();
        ifs.close();

        Json::Value root;
        Json::Reader reader;
        bool ret = reader.parse(str,root);
        if(!ret)
        {
            return false;//解析字符串失败
        }
        _user = root["user"].asString();
        _database = root["database"].asString();
        _ip = root["ip"].asString();
        _port = root["port"].asUInt();
        _password = root["password"].asString();
        return true;
    }


private:
    std::string _file;
    std::string _user;
    std::string _password;
    std::string _database;
    std::string _ip;
    size_t _port;
};