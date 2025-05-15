#pragma once
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Value.hpp>
#include <functional>
#include "logger.hpp"

namespace btyGoose
{
//服务注册类
class Registry{
public:
    Registry(const std::string& etcd_host)
    :_client(std::make_shared<etcd::Client>(etcd_host))
    ,_keep_alive(_client->leasekeepalive(3).get())
    ,_lease_id(_keep_alive->Lease())        //开始续租
    {}

    ~Registry() { _keep_alive->Cancel(); }//析构函数自动停止续租

    bool registerService(const std::string&key,const std::string& value)
    {
        auto resp = _client->put(key,value,_lease_id).get();
        if(resp.is_ok() == false)//如果失败了
        {
            SUP_LOG_ERROR("注册服务失败: {}",resp.error_message());
            return false;
        }
        //成功注册
        return true;
    }

    uint64_t leaseId()
    {
        return _lease_id;
    }
private:
    std::shared_ptr<etcd::Client> _client;
    std::shared_ptr<etcd::KeepAlive> _keep_alive;
    uint64_t _lease_id;
};

class Discovery
{
public:
    using NotifyCallback= std::function<void(std::string,std::string)> ;

private:
    NotifyCallback _put_callback;
    NotifyCallback _del_callback;
    std::shared_ptr<etcd::Client> _client;
    std::shared_ptr<etcd::Watcher> _watcher;
};


}