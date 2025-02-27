#pragma once

#include <string>
#include <odb/core.hxx>  // 重要喵！ODB核心头文件

#pragma db object table("users")
class User {
public:
    User() {}
    User(const std::string& name, int id) : name_(name), id_(id) {}

    #pragma db id
    int id_;
    std::string name_;
};

#pragma db member(User::name_) column("name")
#pragma db member(User::id_) column("id")

void queryAllUsers(MySQLPool& pool) {
    try {
        // 🎣 获取连接守护（自动归还）
        ConnectionGuard guard(pool);
        odb::database& db = *guard;  // 获取数据库引用

        // 🌀 开启事务（重要喵！）
        odb::transaction t(db.begin());

        // 🔍 构建全量查询
        typedef odb::query<User> Query;
        odb::result<User> result = db.query<User>(Query::true_expr);

        // 🐾 遍历结果集
        std::cout << "喵喵用户列表ฅ^•ﻌ•^ฅ：" << std::endl;
        for (const User& user : result) {
            std::cout << "ID: " << user.id_ 
                      << " | 名字: " << user.name_ << std::endl;
        }

        t.commit();  // ✅ 提交事务
    } catch (const odb::exception& e) {
        std::cerr << "数据库异常喵！(＞ω＜) : " << e.what() << std::endl;
    }
}
