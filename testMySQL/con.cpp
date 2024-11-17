#include <iostream>
#include <memory>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

int main() {
    try {
        // 创建 MySQL 驱动程序
        sql::mysql::MySQL_Driver *driver;
        driver = sql::mysql::get_mysql_driver_instance();

        // 连接到 MySQL 数据库
        std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "conn", "12345678"));
        con->setSchema("testDB");//设置数据库

        // 创建一个查询语句
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT VERSION()"));

        // 输出版本信息
        while (res->next()) {
            std::cout << "MySQL Version: " << res->getString(1) << std::endl;
        }

        res->close();

        stmt->executeUpdate("insert into t1 values(255,'神奇')");

        res.reset(stmt->executeQuery("select * from t1"));
        auto rsm(res->getMetaData());
        std::cout<<rsm->getColumnCount()<<std::endl;

        while(res->next())
        {
            std::cout<<res->getRow()<<std::endl;
        }

    } catch (sql::SQLException &e) {
        std::cerr << "SQLException: " << e.what() << std::endl;
        std::cerr << "Error Code: " << e.getErrorCode() << std::endl;
        std::cerr << "SQLState: " << e.getSQLState() << std::endl;
        return 1;
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

