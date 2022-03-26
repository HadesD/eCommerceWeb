#include "DashboardCtrl.h"

#include "app_helpers/ApiResponse.hpp"

#include "models/Orders.h"
#include "app_helpers/OrdersMetaData.hpp"

#include "models/Users.h"
#include "app_helpers/UsersMetaData.hpp"

#include "models/Stocks.h"
#include "app_helpers/StocksMetaData.hpp"

#include "models/Transactions.h"

void DashboardCtrl::statistics(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    using namespace drogon;
    using namespace drogon_model::web_rinphone;
    using namespace app_helpers;

    app_helpers::ApiResponse apiRes;
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        auto dbClient = app().getDbClient();

        auto& resData = apiRes.data();

        auto& orderJson = resData["order"];
        {
            auto ordMapper = orm::Mapper<Orders>(dbClient);
            orderJson["count"] = static_cast<uint>(ordMapper.count(
                orm::Criteria(Orders::Cols::_deleted_at, orm::CompareOperator::IsNull)));
            orderJson["uncompleted_count"] = static_cast<uint>(ordMapper.count(
                orm::Criteria(Orders::Cols::_status, orm::CompareOperator::NotIn,
                              std::vector{
                                  static_cast<uint8_t>(OrderStatus::COMPLETED),
                                  static_cast<uint8_t>(OrderStatus::CANCELED),
                              }) &&
                orm::Criteria(Orders::Cols::_deleted_at, orm::CompareOperator::IsNull)));
        }

        auto& userJson = resData["user"];
        {
            auto userMapper = orm::Mapper<Users>(dbClient);
            userJson["count"] = static_cast<uint>(userMapper.count(
                orm::Criteria(Users::Cols::_deleted_at, orm::CompareOperator::IsNull)));
            userJson["admin_count"] = static_cast<uint>(userMapper.count(
                orm::Criteria(Users::Cols::_deleted_at, orm::CompareOperator::IsNull) &&
                orm::Criteria(Users::Cols::_role, orm::CompareOperator::GE,
                              static_cast<uint8_t>(UserRole::ADMIN_MANAGER))));
        }

        auto& stockJson = resData["stock"];
        {
            auto stockMapper = orm::Mapper<Stocks>(dbClient);
            stockJson["count"] = static_cast<uint>(stockMapper.count(
                orm::Criteria(Stocks::Cols::_deleted_at, orm::CompareOperator::IsNull)));
            stockJson["avail_count"] = static_cast<uint>(stockMapper.count(
                orm::Criteria(Stocks::Cols::_deleted_at, orm::CompareOperator::IsNull) &&
                orm::Criteria(Stocks::Cols::_quantity, orm::CompareOperator::GT, 0)));
            stockJson["avail_cost_price"] = dbClient->execSqlSync(
                                                        "SELECT SUM(quantity * cost_price) aggregate FROM stocks WHERE deleted_at IS NULL AND quantity > 0")[0]
                                                .at("aggregate")
                                                .as<uint>();
        }

        auto& transactionJson = resData["transaction"];
        {
            auto transMapper = orm::Mapper<Transactions>(dbClient);
            transactionJson["funds"] = 600000000;

            auto& chartTotalJson = transactionJson["chart_total"] = Json::Value(Json::arrayValue);

            auto totalRes = dbClient->execSqlSync(
                "SELECT (YEAR(paid_date)*100+MONTH(paid_date)) ym, SUM(amount) amount FROM transactions WHERE deleted_at IS NULL GROUP BY ym ORDER BY ym ASC");
            for (const auto& row : totalRes)
            {
                auto rowJson = Json::Value(Json::objectValue);
                for (const auto& col : row)
                {
                    rowJson[col.name()] = col.as<uint>();
                }
                chartTotalJson.append(rowJson);
            }

            auto& chartNear30dJson = transactionJson["chart_near_30_days"] = Json::Value(Json::arrayValue);
            auto chartNear30d = dbClient->execSqlSync(
                "SELECT (YEAR(paid_date)*10000+MONTH(paid_date)*100+DAY(paid_date)) ymd, SUM(amount) amount FROM transactions WHERE deleted_at IS NULL AND paid_date >= DATE_SUB(NOW(), INTERVAL 30 DAY) GROUP BY ymd ORDER BY ymd ASC");
            for (const auto& row : chartNear30d)
            {
                auto rowJson = Json::Value(Json::objectValue);
                for (const auto& col : row)
                {
                    rowJson[col.name()] = col.as<uint>();
                }
                chartNear30dJson.append(rowJson);
            }

            transactionJson["amount_total"] = dbClient->execSqlSync(
                                                          "SELECT SUM(amount) aggregate FROM transactions WHERE deleted_at IS NULL")[0]
                                                  .at("aggregate")
                                                  .as<uint>();
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
