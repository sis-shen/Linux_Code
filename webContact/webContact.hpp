
#include "contact.pb.h"
#include "httplib.h"

class WebContact
{
public:
    WebContact* getInstance()
    {
        if(ins == nullptr)
        {
            ins = new WebContact;
        }
        return ins;
    }

private:
    WebContact()
    :ins(nullptr),
    server()
    {

    };

    WebContact(const WebContact&) = delete;
    WebContact operator(const WebContact&) = delete;

    WebContact* ins;
private:
    httplib::Server server;
};