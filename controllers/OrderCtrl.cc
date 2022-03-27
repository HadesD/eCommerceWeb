#include "OrderCtrl.h"

#include "models/Products.h"
#include "models/OrderProducts.h"
#include "models/Users.h"

#include "app_helpers/OrdersMetaData.hpp"
#include "app_helpers/UsersMetaData.hpp"
#include "app_helpers/ApiResponse.hpp"
#include "app_helpers/ApiRequestParse.hpp"

using Product = drogon_model::web_rinphone::Products;
using Order = drogon_model::web_rinphone::Orders;
using OrderProduct = drogon_model::web_rinphone::OrderProducts;
using User = drogon_model::web_rinphone::Users;

using namespace app_helpers;

Task<> OrderCtrl::get(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
{
    auto dbClient = app().getDbClient();

    orm::Criteria cnd(Order::Cols::_deleted_at, orm::CompareOperator::IsNull);

    orm::SortOrder orderSort{orm::SortOrder::DESC};
    auto orderBy = Order::Cols::_created_at;

    // Sort
    const auto &reqSortBy = req->getParameter("sort_by");
    if (reqSortBy.size() >= 2)
    {
        const auto &tmpSortBy = reqSortBy.substr(1);
        static std::vector<std::string> allowCols{
            Order::Cols::_created_at,
            Order::Cols::_deal_date,
        };
        if (std::find(allowCols.cbegin(), allowCols.cend(), tmpSortBy) != allowCols.cend())
        {
            orderSort = (reqSortBy[0] == '+') ? orm::SortOrder::ASC : orm::SortOrder::DESC;
            orderBy = tmpSortBy;
        }
    }

    app_helpers::ApiResponse apiRes;
    auto &resJson = apiRes.json();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        size_t page = getPage(req);
        size_t limit = 24;

        //! TODO: move to CoroMapper
        orm::Mapper<Order> ordMap(dbClient);
        const auto &ords = ordMap.orderBy(orderBy, orderSort).paginate(page, limit).findBy(cnd);
        auto &retData = apiRes.data();
        retData = Json::Value(Json::arrayValue);
        for (const auto &ord : ords)
        {
            app_helpers::orderJsonRow(dbClient, ord, retData.append(ord.toJson()));
        }

        apiRes.appendPaginate(page, limit, ordMap.count(cnd));
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);
    callback(httpRet);

    co_return;
}

void OrderCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
    callback(HttpResponse::newNotFoundResponse());
}

void OrderCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{

}

void OrderCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{

}
