#include <drogon/drogon.h>
#include <jsoncpp/json/json.h>
using namespace drogon;
int main()
{
    //注册路由
    app().registerHandler("/test?username={name}",
        [](const HttpRequestPtr& req,
           std::function<void (const HttpResponsePtr &)> &&callback,
           const std::string &name)
        {
            Json::Value json;
            json["result"]="ok";
            json["message"]=std::string("hello,")+name;
            auto resp=HttpResponse::newHttpJsonResponse(json);
            callback(resp);
        },
        {Get,"LoginFilter"});
    //启动服务器
    app().setLogPath("./")
         .setLogLevel(trantor::Logger::kWarn)
         .addListener("0.0.0.0", 8088)
         .setThreadNum(16)
        //  .enableRunAsDaemon()
         .run();
}