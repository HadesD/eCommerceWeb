#pragma once

#include "models/Products.h"

namespace app_helpers
{
    enum class ProductStatus : uint8_t
    {
        DRAFT = 0,
        SELLING = 1,
        SOLDOUT = 2,
    };

    void productJsonRow(const drogon::orm::DbClientPtr &dbClient,
                        const drogon_model::web_rinphone::Products &prd, Json::Value &prdRow);
}
