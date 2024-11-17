#include "DBUserConfig.hpp"
#include <iostream>
#include <memory>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

using namespace std;

int main()
{
    DBUserConfig dbu;//创建用户配置类
    bool ret = dbu.readFile();//试图读取配置文件
    if(!ret)
    {
        cout<<"start write\n";
        dbu.setFile("./userConfig");
        dbu.setIp("127.0.0.1");
        dbu.setPort(3306);
        dbu.setUser("conn");
        dbu.setDatabase("testDB");
        dbu.setPassword("12345678");
    }
    
    sql::mysql::MySQL_Driver* driver;//创建mysql驱动程序
    driver = sql::mysql::get_mysql_driver_instance();//获取实例

    //连接数据库
    std::unique_ptr<sql::Connection> conn(driver->connect(dbu.Address(),dbu.User(),dbu.Password()));

    cout<<conn->getClientInfo()<<endl;//输出连接状态
    cout<<"is connect: "<< (!conn->isClosed());

    dbu.writeFile();//将配置写入文件
    return 0;
}