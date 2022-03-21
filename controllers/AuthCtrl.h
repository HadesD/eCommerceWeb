#pragma once
#include <drogon/HttpController.h>

using namespace drogon;

class AuthCtrl : public drogon::HttpController<AuthCtrl>
{
  public:
    METHOD_LIST_BEGIN
    //use METHOD_ADD to add your custom processing function here;
    //METHOD_ADD(AuthCtrl::get,"/{2}/{1}",Get);//path is /AuthCtrl/{arg2}/{arg1}
    //METHOD_ADD(AuthCtrl::your_method_name,"/{1}/{2}/list",Get);//path is /AuthCtrl/{arg1}/{arg2}/list
    ADD_METHOD_TO(AuthCtrl::login, "/login", Post);
    ADD_METHOD_TO(AuthCtrl::logout, "/logout", Post);
    ADD_METHOD_TO(AuthCtrl::user, "/user", Get);

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    Task<> login(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback);
    Task<> logout(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback);
    Task<> user(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback);
};
