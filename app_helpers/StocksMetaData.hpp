#pragma once

#include "models/Stocks.h"
#include "models/Products.h"
#include "models/Categories.h"
#include "models/StockCategories.h"
#include "models/Images.h"
#include "models/StockImages.h"

namespace app_helpers
{
    using Stock = drogon_model::web_rinphone::Stocks;

    void stockJsonRow(const drogon::orm::DbClientPtr &dbClient, const Stock& stk, Json::Value &stkRow)
    {
        using Category = drogon_model::web_rinphone::Categories;
        using ProductCategory = drogon_model::web_rinphone::ProductCategories;
        using Image = drogon_model::web_rinphone::Images;
        using ProductImage = drogon_model::web_rinphone::ProductImages;

        // categories
        {
            auto &stkCatRow = stkRow[Category::tableName];
            stkCatRow = Json::Value(Json::arrayValue);
            std::promise<int> pCatRet;
            auto fCatRet = pCatRet.get_future();
            stk.getCategory(
                dbClient,
                [&stkCatRow, &pCatRet](auto pairRows)
                {
                    for (const auto &pairRow : pairRows)
                    {
                        stkCatRow.append(pairRow.first.toJson());
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

        // images
        {
            auto& stkImgRow = stkRow[Image::tableName];
            stkImgRow = Json::Value(Json::arrayValue);
            std::promise<int> pImgRet;
            auto fImgRet = pImgRet.get_future();
            stk.getImage(
                dbClient,
                [&stkImgRow, &pImgRet](auto pairRows)
                {
                    for (const auto &pairRow : pairRows)
                    {
                        stkImgRow.append(pairRow.first.toJson());
                    }

                    pImgRet.set_value(0);
                },
                [&pImgRet](const auto &e)
                {
                    LOG_ERROR << e.base().what();

                    pImgRet.set_value(1);
                });
            fImgRet.get();
        }
    }
}
