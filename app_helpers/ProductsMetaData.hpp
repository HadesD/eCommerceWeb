#pragma once

#include "models/Products.h"
#include "models/Categories.h"
#include "models/ProductCategories.h"

namespace app_helpers
{
    using Product = drogon_model::web_rinphone::Products;

    enum class ProductStatus : char
    {
        DRAFT = 0,
        SELLING,
        SOLDOUT,
    };

    void productJsonRow(const drogon::orm::DbClientPtr &dbClient, const Product& prd, Json::Value &prdRow)
    {
        using Category = drogon_model::web_rinphone::Categories;
        using ProductCategory = drogon_model::web_rinphone::ProductCategories;

        auto &prdCatRow = prdRow[Category::tableName];
        prdCatRow = Json::Value(Json::arrayValue);
        std::promise<int> pCatRet;
        auto fCatRet = pCatRet.get_future();
        prd.getCategory(
            dbClient,
            [&prdCatRow, &pCatRet](auto pairRows)
            {
                for (const auto &pairRow : pairRows)
                {
                    prdCatRow.append(pairRow.first.toJson());
                }

                pCatRet.set_value(0);
            },
            [&pCatRet](const auto &e)
            {
                LOG_ERROR << e.base().what();

                pCatRet.set_value(1);
            });
        fCatRet.get();
    }
}
