#include "OrderCtrl.h"

#include "models/Products.h"
#include "models/Orders.h"
#include "models/OrderProducts.h"
#include "models/Users.h"

#include "app_helpers/OrdersMetaData.hpp"
#include "app_helpers/UsersMetaData.hpp"

using Product = drogon_model::web_rinphone::Products;
using Order = drogon_model::web_rinphone::Orders;
using OrderProduct = drogon_model::web_rinphone::OrderProducts;
using User = drogon_model::web_rinphone::Users;
using namespace app_helpers;

void OrderCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    callback(HttpResponse::newNotFoundResponse());
}

void OrderCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
    callback(HttpResponse::newNotFoundResponse());
}

