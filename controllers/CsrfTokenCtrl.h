#pragma once
#include <drogon/HttpSimpleController.h>
using namespace drogon;
class CsrfTokenCtrl:public drogon::HttpSimpleController<CsrfTokenCtrl>
{
  public:
    virtual void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    //list path definitions here;
    PATH_ADD("/csrf-token", Get);
    PATH_LIST_END

    static std::string getToken(const HttpRequestPtr& req);
    static bool verify(const HttpRequestPtr& req);
};
