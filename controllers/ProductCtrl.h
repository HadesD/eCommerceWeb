#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class ProductCtrl : public drogon::HttpController<ProductCtrl>
{
  public:
    METHOD_LIST_BEGIN
    //use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(ProductCtrl::get, "/", Get); //path is /ProductCtrl/{arg2}/{arg1}
    //METHOD_ADD(ProductCtrl::your_method_name,"/{1}/{2}/list",Get);//path is /ProductCtrl/{arg1}/{arg2}/list
    //ADD_METHOD_TO(ProductCtrl::your_method_name,"/absolute/path/{1}/{2}/list",Get);//path is /absolute/path/{arg1}/{arg2}/list

    ADD_METHOD_TO(ProductCtrl::get, "/products", Get); //path is /absolute/path/{arg1}/{arg2}/list
    ADD_METHOD_TO(ProductCtrl::getOne, "/products/{1}", Get); //path is /absolute/path/{arg1}/{arg2}/list

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    // void your_method_name(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback,double p1,int p2) const;

    void get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id);
};
