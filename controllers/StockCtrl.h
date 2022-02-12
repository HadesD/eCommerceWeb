#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class StockCtrl : public drogon::HttpController<StockCtrl>
{
public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(StockCtrl::get,"/{2}/{1}",Get);//path is /StockCtrl/{arg2}/{arg1}
    // METHOD_ADD(StockCtrl::your_method_name,"/{1}/{2}/list",Get);//path is /StockCtrl/{arg1}/{arg2}/list
    // ADD_METHOD_TO(StockCtrl::your_method_name,"/absolute/path/{1}/{2}/list",Get);//path is /absolute/path/{arg1}/{arg2}/list

    ADD_METHOD_TO(StockCtrl::get, "/stocks", Get);
    ADD_METHOD_TO(StockCtrl::getOne, "/stocks/{1}", Get);
    ADD_METHOD_TO(StockCtrl::create, "/stocks", Post);
    ADD_METHOD_TO(StockCtrl::updateOne, "/stocks/{1}", Post);

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,int p1,std::string p2);
    // void your_method_name(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,double p1,int p2) const;

    void get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id);
    void create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id);
};
