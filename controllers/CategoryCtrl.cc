#include "CategoryCtrl.h"

#include "models/Categories.h"

// add definition of your processing function here

using Category = drogon_model::web_rinphone::Categories;

void CategoryCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = drogon::app().getDbClient();

    Json::Value ret;

    try
    {
        drogon::orm::Mapper<Category> catMap(dbClient);
        auto cats = catMap.orderBy(Category::Cols::_parent_id).orderBy(Category::Cols::_name).findAll();

        auto &retData = ret["data"];
        retData = Json::Value(Json::arrayValue);

        for (const auto &cat : cats)
        {
            retData.append(cat.toJson());
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        auto res = HttpResponse::newHttpResponse();
        res->setStatusCode(HttpStatusCode::k500InternalServerError);

        callback(res);

        return;
    }

    callback(HttpResponse::newHttpJsonResponse(ret));
}
