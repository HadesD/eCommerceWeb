#include "OrdersMetaData.hpp"

#include "models/Products.h"
#include "models/OrderProducts.h"
#include "models/OrderProductStocks.h"
#include "models/OrderProductStockTransactions.h"

namespace app_helpers
{
    void orderJsonRow(const drogon::orm::DbClientPtr &dbClient, const drogon_model::web_rinphone::Orders &order, Json::Value &row)
    {
        using namespace drogon;
        using namespace drogon_model::web_rinphone;

        // order_products
        {
            auto& orderProducts = row[OrderProducts::tableName] = Json::Value(Json::arrayValue);
            orm::Mapper<OrderProducts> orderProductsMapper(dbClient);
            const auto& ops = orderProductsMapper.findBy(orm::Criteria(OrderProducts::Cols::_order_id, order.getValueOfId()));
            for (const auto& op : ops)
            {
                orderProducts.append(op.toJson());
            }
        }
    }
}
