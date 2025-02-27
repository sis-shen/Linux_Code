#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <odb/mysql/database.hxx>

class MySQLPool {
public:
    // 🐾 初始化连接池
    MySQLPool(const std::string& user, 
             const std::string& password,
             const std::string& db,
             size_t poolSize = 10)
        : maxSize_(poolSize) 
    {
        for(size_t i=0; i<poolSize; ++i){
            connections_.emplace(
                std::make_shared<odb::mysql::database>(
                    user, password, db, "localhost"
                )
            );
        }
    }

    // 🎣 获取连接（带超时）
    std::shared_ptr<odb::database> getConnection(int timeoutMs = 30000) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(cond_.wait_for(lock, std::chrono::milliseconds(timeoutMs), 
            [this]{ return !connections_.empty(); }))
        {
            auto conn = connections_.front();
            connections_.pop();
            return conn;
        }
        throw std::runtime_error("获取连接超时喵！(＞﹏＜)");
    }

    // 🐟 归还连接
    void returnConnection(std::shared_ptr<odb::database> conn) {
        std::unique_lock<std::mutex> lock(mutex_);
        connections_.push(conn);
        cond_.notify_one();
    }

private:
    std::queue<std::shared_ptr<odb::database>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
    size_t maxSize_;
};

// ⏳ 带作用域的自动归还（RAII模式）
class ConnectionGuard {
    public:
        ConnectionGuard(MySQLPool& p) : pool(p), conn(p.getConnection()) {}
        ~ConnectionGuard() { pool.returnConnection(conn); }
        odb::database& operator*() { return *conn; }
    private:
        MySQLPool& pool;
        std::shared_ptr<odb::database> conn;
    };