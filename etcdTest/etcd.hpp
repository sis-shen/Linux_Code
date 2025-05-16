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
        SUP_LOG_DEBUG("成功注册服务: {}-{}",key,value);
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

    Discovery(
        const std::string& etcd_host,
        const std::string& basedir,
        const NotifyCallback& put_cb,
        const NotifyCallback& del_cb 
    )
    :_client(std::make_shared<etcd::Client>(etcd_host))
    ,_put_callback(put_cb)
    ,_del_callback(del_cb)
    {
        //先获取已有的服务
        auto resp = _client->ls(basedir).get();
        if(resp.is_ok() == false)
        {
            SUP_LOG_ERROR("Discovery获取服务信息失败: {}",resp.error_message());
        }
        int sz = resp.keys().size();
        SUP_LOG_INFO("Discovery初始化发现已有服务数： {}",sz);
        for(int i = 0;i<sz;++i)
        {
            if(_put_callback)//如果设置了回调函数
            {
                _put_callback(resp.key(i),resp.value(i).as_string());
                SUP_LOG_DEBUG("Discovery初始化时，发现服务： {}->{}",resp.key(i),resp.value(i).as_string());
            }
        }
        //然后监控后续的变化,注意recursive要为true
        _watcher = std::make_shared<etcd::Watcher>(*(_client.get()),basedir,
            std::bind(&Discovery::callback,this,std::placeholders::_1),true);
    }

    ~Discovery()
    {
        _watcher->Cancel();//取消监控
    }

    void callback(const etcd::Response &resp)
    {
        if(resp.is_ok() == false)
        {
            SUP_LOG_ERROR("收到一个错误事件的通知： {}",resp.error_message());
            return;
        }

        for(auto const&ev:resp.events())
        {
            if(ev.event_type() == etcd::Event::EventType::PUT)
            {
                //如果设置了回调函数
                if(_put_callback)
                {
                    _put_callback(ev.kv().key(),ev.kv().as_string());
                    SUP_LOG_DEBUG("新增服务: {}->{}",ev.kv().key(),ev.kv().as_string());
                }
            }
            else if (ev.event_type() == etcd::Event::EventType::DELETE_)
            {
                //如果设置了回调函数
                if(_del_callback)
                {
                    _del_callback(ev.kv().key(),ev.kv().as_string());
                    SUP_LOG_DEBUG("下线服务： {}",ev.kv().key());
                }
            }
        }
    }

private:
    NotifyCallback _put_callback;
    NotifyCallback _del_callback;
    std::shared_ptr<etcd::Client> _client;
    std::shared_ptr<etcd::Watcher> _watcher;
};

}