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

    try
    {
        auto dbClient = app().getDbClient()->newTransaction();
        for (const auto& item : reqJson)
        {
            LOG_DEBUG << item["num"].asInt();
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        auto ret = HttpResponse::newHttpResponse();
        ret->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(ret);
        return;
    }

    callback(HttpResponse::newHttpJsonResponse(resJson));
}
