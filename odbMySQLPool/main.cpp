#include "MySQLPoll.hpp"

#include <iostream>
#include <odb/core.hxx>
#include <odb/mysql/database.hxx>
#include <odb/query.hxx>
#include <odb/result.hxx>
#include <odb/transaction.hxx>


int main() {
    // 🏊 初始化连接池（参数根据实际情况修改）
    MySQLPool pool("root", "", "testDB", 5);

    // 🚀 执行查询
    queryAllUsers(pool);

    return 0;
}