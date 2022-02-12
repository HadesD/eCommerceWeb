#include "StockCtrl.h"
//add definition of your processing function here

#include "models/Stocks.h"
#include "models/Categories.h"
#include "models/StockCategories.h"

#include "app_helpers/StocksMetaData.hpp"

using Stock = drogon_model::web_rinphone::Stocks;
using Category = drogon_model::web_rinphone::Categories;
using StockCategory = drogon_model::web_rinphone::StockCategories;
using Image = drogon_model::web_rinphone::Images;
using StockImage = drogon_model::web_rinphone::StockImages;

void StockCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = app().getDbClient();
    orm::Mapper<Stock> stkMap(dbClient);

    Json::Value resJson;
    auto& resMsg = resJson["message"];
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    orm::Criteria cnd(Stock::Cols::_deleted_at, orm::CompareOperator::IsNull);

    try
    {
        size_t page;
        size_t limit = 24;

        try
        {
            page = std::stoul(req->getParameter("page"));
            page = (page < 1) ? 1 : page;
        }
        catch (...)
        {
            page = 1;
        }

        const auto &stks = stkMap
                               .orderBy(Stock::Cols::_created_at, orm::SortOrder::DESC)
                               .paginate(page, limit)
                               .findBy(cnd);
        auto &retData = resJson["data"];
        retData = Json::Value(Json::arrayValue);
        for (const auto &stk : stks)
        {
            app_helpers::stockJsonRow(dbClient, stk, retData.append(stk.toJson()));
        }

        resJson["total"] = static_cast<uint>(stkMap.count(cnd));
        resJson["current_page"] = static_cast<uint>(page);
        resJson["per_page"] = static_cast<uint>(limit);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    auto res = HttpResponse::newHttpJsonResponse(resJson);
    res->setStatusCode(httpRetCode);
    callback(res);
}

void StockCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{

}

void StockCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{

}

void StockCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{

}
