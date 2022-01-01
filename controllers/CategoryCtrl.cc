#include "CategoryCtrl.h"

#include "models/Categories.h"

// add definition of your processing function here

using Category = drogon_model::web_rinphone::Categories;

void CategoryCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = drogon::app().getDbClient();

    Json::Value ret;

    // TODO: Fix not to use bellow conditional
    drogon::orm::Criteria catCond(Category::Cols::_id, drogon::orm::CompareOperator::IsNotNull);

    try
    {
        auto cat = drogon::orm::Mapper<Category>(dbClient)
                       .findOne(drogon::orm::Criteria(Category::Cols::_slug, req->getParameter("parent_slug")));
        catCond = catCond && drogon::orm::Criteria(Category::Cols::_parent_id, cat.getValueOfId());

        ret["name"] = cat.getValueOfName();
    }
    catch (const drogon::orm::UnexpectedRows& e)
    {
    }
    catch (const std::exception &e)
    {
        LOG_DEBUG << e.what();
    }

    try
    {
        drogon::orm::Mapper<Category> catMap(dbClient);
        auto cats = catMap.findBy(catCond);

        auto &retData = ret["data"];
        retData = Json::Value(Json::arrayValue);

        for (const auto &cat : cats)
        {
            retData.append(cat.toJson());
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << e.what();

        auto res = HttpResponse::newHttpResponse();
        res->setStatusCode(HttpStatusCode::k500InternalServerError);

        callback(res);

        return;
    }

    callback(HttpResponse::newHttpJsonResponse(ret));
}
