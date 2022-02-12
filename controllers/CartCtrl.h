#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class CartCtrl : public drogon::HttpController<CartCtrl>
{
public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(CartCtrl::get,"/{2}/{1}",Get);//path is /CartCtrl/{arg2}/{arg1}
    // METHOD_ADD(CartCtrl::your_method_name,"/{1}/{2}/list",Get);//path is /CartCtrl/{arg1}/{arg2}/list
    // ADD_METHOD_TO(CartCtrl::your_method_name,"/absolute/path/{1}/{2}/list",Get);//path is /absolute/path/{arg1}/{arg2}/list

    ADD_METHOD_TO(CartCtrl::checkout, "/checkout");

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,int p1,std::string p2);
    // void your_method_name(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,double p1,int p2) const;

    void checkout(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
};
