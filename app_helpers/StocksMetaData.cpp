#include "models/Stocks.h"
#include "models/Products.h"
#include "models/Categories.h"
#include "models/StockCategories.h"
#include "models/Images.h"
#include "models/StockImages.h"
#include "models/Users.h"
#include "models/Transactions.h"
#include "models/StockTransactions.h"

namespace app_helpers
{
    void stockJsonRow(const drogon::orm::DbClientPtr &dbClient,
                      const drogon_model::web_rinphone::Stocks &stk, Json::Value &stkRow)
    {
        using Category = drogon_model::web_rinphone::Categories;
        using Image = drogon_model::web_rinphone::Images;
        using Transaction = drogon_model::web_rinphone::Transactions;

        // categories
        {
            auto &stkCatRow = stkRow[Category::tableName];
            stkCatRow = Json::Value(Json::arrayValue);
            std::promise<uint8_t> pCatRet;
            auto fCatRet = pCatRet.get_future();
            stk.getCategories(
                dbClient,
                [&stkCatRow, &pCatRet](const auto& pairRows)
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
            std::promise<uint8_t> pImgRet;
            auto fImgRet = pImgRet.get_future();
            stk.getImages(
                dbClient,
                [&stkImgRow, &pImgRet](const auto& pairRows)
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

        // images
        {
            auto& stkTxnRow = stkRow[Transaction::tableName];
            stkTxnRow = Json::Value(Json::arrayValue);
            std::promise<uint8_t> pTxnRet;
            auto fTxnRet = pTxnRet.get_future();
            stk.getTransactions(
                dbClient,
                [&stkTxnRow, &pTxnRet](const auto& pairRows)
                {
                    for (const auto &pairRow : pairRows)
                    {
                        stkTxnRow.append(pairRow.first.toJson());
                    }

                    pTxnRet.set_value(0);
                },
                [&pTxnRet](const auto &e)
                {
                    LOG_ERROR << e.base().what();

                    pTxnRet.set_value(1);
                });
            fTxnRet.get();
        }

        // tester
        {
            auto& stkImgRow = stkRow["tester"];
            stkImgRow = Json::Value(Json::objectValue);
            std::promise<uint8_t> pImgRet;
            auto fImgRet = pImgRet.get_future();
            stk.getTester(
                dbClient,
                [&stkImgRow, &pImgRet](const auto &row)
                {
                    stkImgRow = row.toJson();

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
