#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class DashboardCtrl : public drogon::HttpController<DashboardCtrl>
{
  public:
    METHOD_LIST_BEGIN
    // use METHOD_ADD to add your custom processing function here;
    // METHOD_ADD(DashboardCtrl::get, "/{2}/{1}", Get); // path is /DashboardCtrl/{arg2}/{arg1}
    // METHOD_ADD(DashboardCtrl::your_method_name, "/{1}/{2}/list", Get); // path is /DashboardCtrl/{arg1}/{arg2}/list
    ADD_METHOD_TO(DashboardCtrl::statistics, "/statistics", Get);

    METHOD_LIST_END
    // your declaration of processing function maybe like this:
    // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
    void statistics(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) const;
};
