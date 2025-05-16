#include "etcd.hpp"
#include <unordered_map>
//服务端的url
const string& etcd_host = "http://127.0.0.1:2379";
std::unordered_map<std::string,std::string> service_table;

void put_cb(std::string key,std::string value)
{
    service_table[key] = value;
    SUP_LOG_INFO("已存入服务列表: {}->{}",key,value);
}
void del_cb(std::string key,std::string value)
{
    service_table.erase(key);
    SUP_LOG_INFO("已下线服务: {}",key);
}

void testRegister()
{
    btyGoose::Registry* rs = new btyGoose::Registry(etcd_host);
    rs->registerService("example_service1-1","http://example.ex:8888");
    rs->registerService("example_service1-2","http://example.ex:9999");
    rs->registerService("example_service2-1","http://example2.ex:8888");

    btyGoose::Discovery dc(etcd_host,"",put_cb,del_cb);
    SUP_LOG_INFO("Discovery已创建");
    sleep(1);

    rs->registerService("example_service3-3","http://testDiscovery.ex:8888");

    for(auto [x,y]:service_table)
    {
        SUP_LOG_DEBUG("Register析构前含有服务: {}->{}",std::string(x),std::string(y));
    }
    delete rs;

    SUP_LOG_INFO("接下来等待10秒...");
    
    sleep(10);
    int size = service_table.size();
    SUP_LOG_INFO("睡醒了！来看看还剩多少服务记录: {}",size);
}

int main()
{
    std::cerr<<"test\n";
    btyGoose::init_logger();
    SUP_LOG_INFO("开始测试");
    testRegister();


    return 0;
}