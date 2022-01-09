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
    const auto& reqJsonPtr = req->getJsonObject();
    if (!reqJsonPtr || !(*reqJsonPtr).isArray())
    {
        auto ret = HttpResponse::newHttpResponse();
        ret->setStatusCode(HttpStatusCode::k406NotAcceptable);
        callback(ret);
        return;
    }

    const auto& reqJson = *reqJsonPtr;

    Json::Value resJson;

    callback(HttpResponse::newHttpJsonResponse(resJson));
}
