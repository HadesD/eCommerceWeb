#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class OrderCtrl : public drogon::HttpController<OrderCtrl>
{
  public:
    METHOD_LIST_BEGIN
    //use METHOD_ADD to add your custom processing function here;
    //METHOD_ADD(OrderCtrl::get,"/{2}/{1}",Get);//path is /OrderCtrl/{arg2}/{arg1}
    //METHOD_ADD(OrderCtrl::your_method_name,"/{1}/{2}/list",Get);//path is /OrderCtrl/{arg1}/{arg2}/list
    //ADD_METHOD_TO(OrderCtrl::your_method_name,"/absolute/path/{1}/{2}/list",Get);//path is /absolute/path/{arg1}/{arg2}/list

    ADD_METHOD_TO(OrderCtrl::get, "/orders", Get, "AuthFilter");
    ADD_METHOD_TO(OrderCtrl::getOne, "/orders/{1}", Get, "AuthFilter");
    ADD_METHOD_TO(OrderCtrl::create, "/orders", Post, "AuthFilter");
    ADD_METHOD_TO(OrderCtrl::updateOne, "/orders/{1}", Post, "AuthFilter");

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,int p1,std::string p2);
    // void your_method_name(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,double p1,int p2) const;

    Task<> get(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback);
    void getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id);
    void create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id);
};
