#include "OrderCtrl.h"
//add definition of your processing function here

void OrderCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    callback(HttpResponse::newNotFoundResponse());
}

void OrderCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
    callback(HttpResponse::newNotFoundResponse());
}

void OrderCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    LOG_DEBUG << 22;
    const auto reqJson = req->getJsonObject();
    if (!reqJson)
    {
        auto ret = HttpResponse::newHttpResponse();
        ret->setStatusCode(HttpStatusCode::k406NotAcceptable);
        callback(ret);
        return;
    }

    Json::Value resJson;

    callback(HttpResponse::newHttpJsonResponse(resJson));
}
