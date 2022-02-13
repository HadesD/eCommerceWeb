#pragma once

#include "models/Products.h"
#include "models/Categories.h"
#include "models/ProductCategories.h"
#include "models/Images.h"
#include "models/ProductImages.h"

namespace app_helpers
{
    using Product = drogon_model::web_rinphone::Products;

    enum class ProductStatus : char
    {
        DRAFT = 0,
        SELLING = 1,
        SOLDOUT = 2,
    };

    void productJsonRow(const drogon::orm::DbClientPtr &dbClient, const Product& prd, Json::Value &prdRow)
    {
        using Category = drogon_model::web_rinphone::Categories;
        using Image = drogon_model::web_rinphone::Images;

        // categories
        {
            auto &prdCatRow = prdRow[Category::tableName];
            prdCatRow = Json::Value(Json::arrayValue);
            std::promise<uint8_t> pCatRet;
            auto fCatRet = pCatRet.get_future();
            prd.getCategories(
                dbClient,
                [&prdCatRow, &pCatRet](const auto& pairRows)
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

        // images
        {
            auto& prdImgRow = prdRow[Image::tableName];
            prdImgRow = Json::Value(Json::arrayValue);
            std::promise<uint8_t> pImgRet;
            auto fImgRet = pImgRet.get_future();
            prd.getImages(
                dbClient,
                [&prdImgRow, &pImgRet](const auto& pairRows)
                {
                    for (const auto &pairRow : pairRows)
                    {
                        prdImgRow.append(pairRow.first.toJson());
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
