#pragma once

#include "models/Orders.h"

namespace app_helpers
{
    enum class OrderStatus
    {
        PROCESSING = 0,
        CANCELED = 10,
        PAID = 50,
        PAYING = 51,
        SHIPPING = 100,
        COMPLETED = 200,
    };

    enum class PaymentMethod
    {
        ONCE = 1,
        INSTALMENT = 2, // Tra gop
    };

    void orderJsonRow(const drogon::orm::DbClientPtr &dbClient, const drogon_model::web_rinphone::Orders &order, Json::Value &row);
}
