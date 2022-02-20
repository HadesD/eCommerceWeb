#include "StockCtrl.h"
//add definition of your processing function here

#include "models/Stocks.h"
#include "models/Categories.h"
#include "models/StockCategories.h"
#include "models/Images.h"
#include "models/StockImages.h"
#include "models/Transactions.h"
#include "models/StockTransactions.h"

#include "app_helpers/StocksMetaData.hpp"
#include "app_helpers/Auth.hpp"

using Stock = drogon_model::web_rinphone::Stocks;
using Category = drogon_model::web_rinphone::Categories;
using StockCategory = drogon_model::web_rinphone::StockCategories;
using Image = drogon_model::web_rinphone::Images;
using StockImage = drogon_model::web_rinphone::StockImages;
using Transaction = drogon_model::web_rinphone::Transactions;
using StockTransaction = drogon_model::web_rinphone::StockTransactions;

void StockCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = app().getDbClient();
    orm::Mapper<Stock> stkMap(dbClient);

    Json::Value resJson;
    auto& resMsg = resJson["message"];
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    orm::Criteria cnd(Stock::Cols::_deleted_at, orm::CompareOperator::IsNull);

    // Category
    {
        Category::PrimaryKeyType catId = 0;
        orm::Mapper<Category> catMap(dbClient);
        const auto &reqCatId = req->getParameter("category_id");
        if (reqCatId.size())
        {
            try
            {
                catId = std::stoul(reqCatId);
            }
            catch (...)
            {
            }
        }
        if (catId)
        {
            std::vector<Category::PrimaryKeyType> catIds{catId};

            std::function<void(const Category::PrimaryKeyType)> findChildCategoryId;
            findChildCategoryId = [&catIds, &catMap, &findChildCategoryId](const Category::PrimaryKeyType parentId)
            {
                auto childCats = catMap.findBy(
                    orm::Criteria(Category::Cols::_parent_id, parentId));
                for (const auto &childCat : childCats)
                {
                    const auto childId = childCat.getValueOfId();
                    catIds.push_back(childId);

                    findChildCategoryId(childId);
                }
            };

            try
            {
                findChildCategoryId(catId);

                std::vector<Stock::PrimaryKeyType> stkIds;
                const auto stkCats = orm::Mapper<StockCategory>(dbClient)
                                             .findBy(orm::Criteria(StockCategory::Cols::_category_id, orm::CompareOperator::In, catIds));

                for (const auto &stkCat : stkCats)
                {
                    stkIds.push_back(stkCat.getValueOfStockId());
                }
                if (!stkIds.size())
                {
                    stkIds.push_back(0);
                }

                cnd = cnd && orm::Criteria(Stock::Cols::_id, orm::CompareOperator::In, stkIds);
            }
            catch (const orm::UnexpectedRows &e)
            {
            }
            catch (const std::exception &e)
            {
                LOG_ERROR << e.what();
            }
        }
    }

    try
    {
        size_t page;
        size_t limit = 24;

        try
        {
            page = std::stoul(req->getParameter("page"));
            page = (page < 1) ? 1 : page;
        }
        catch (...)
        {
            page = 1;
        }

        const auto &stks = stkMap
                               .orderBy(Stock::Cols::_created_at, orm::SortOrder::DESC)
                               .paginate(page, limit)
                               .findBy(cnd);
        auto &retData = resJson["data"];
        retData = Json::Value(Json::arrayValue);
        for (const auto &stk : stks)
        {
            app_helpers::stockJsonRow(dbClient, stk, retData.append(stk.toJson()));
        }

        resJson["total"] = static_cast<uint>(stkMap.count(cnd));
        resJson["current_page"] = static_cast<uint>(page);
        resJson["per_page"] = static_cast<uint>(limit);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    auto res = HttpResponse::newHttpJsonResponse(resJson);
    res->setStatusCode(httpRetCode);
    callback(res);
}

void StockCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
    Json::Value resJson;
    auto &resMsg = resJson["message"];
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        auto dbClient = app().getDbClient();
        const auto &stk = orm::Mapper<Stock>(dbClient)
                              .findByPrimaryKey(id);

        auto &retData = resJson["data"];
        retData = stk.toJson();
        app_helpers::stockJsonRow(dbClient, stk, retData);
    }
    catch (const orm::UnexpectedRows &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k404NotFound;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    auto res = HttpResponse::newHttpJsonResponse(resJson);
    res->setStatusCode(httpRetCode);

    callback(res);
}

void StockCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    Json::Value resJson;
    auto& resMsg = resJson["message"];
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        const auto reqJsonPtr = req->getJsonObject();
        if (!reqJsonPtr)
        {
            throw std::logic_error("Dữ liệu gửi lên không đúng");
        }
        const auto& reqJson = *reqJsonPtr;

        auto dbClient = app().getDbClient()->newTransaction();
        orm::Mapper<Stock> stkMapper{dbClient};
        Stock stk;

        const auto& name = reqJson["name"];
        if (!name.isString())
        {
            throw std::logic_error("Chưa điền tên");
        }
        stk.setName(name.asString());

        const auto& idi = reqJson["idi"];
        if (!idi.isString())
        {
            throw std::logic_error("Chưa điền idi");
        }
        stk.setName(idi.asString());

        const auto& sell_price = reqJson["sell_price"];
        if (!sell_price.isInt())
        {
            throw std::logic_error("Chưa điền giá");
        }
        stk.setSellPrice(sell_price.asInt());

        const auto& cost_price = reqJson["cost_price"];
        if (!cost_price.isInt() || (cost_price.asInt() < 0))
        {
            throw std::logic_error("Chưa điền giá");
        }
        stk.setCostPrice(cost_price.asInt());

        const auto& quantity = reqJson["quantity"];
        if (!quantity.isInt() || (quantity.asInt() < 1))
        {
            throw std::logic_error("Chưa chọn số lượng");
        }
        stk.setQuantity(quantity.asInt());

        const auto& tester_id = reqJson["tester_id"];
        if (!tester_id.isUInt64())
        {
            throw std::logic_error("Chưa chọn người test");
        }
        stk.setTesterId(quantity.asUInt64());

        const auto& category_ids = reqJson["category_ids"];
        if (!category_ids.isArray() || (category_ids.size() < 1))
        {
            throw std::logic_error("Chưa chọn chuyên mục");
        }

        const auto& inout_date = reqJson["inout_date"];
        if (!inout_date.isString())
        {
            throw std::logic_error("Chưa chọn ngày nhập / trả");
        }

        const auto& note = reqJson["note"];
        if (note.isString())
        {
            stk.setNote(note.asString());
        }

        const auto& curUser = app_helpers::Auth::user(req);
        stk.setUpdatedUserId(curUser.getValueOfId());

        const auto now = trantor::Date::now();

        stk.setCreatedAt(now);
        stk.setUpdatedAt(now);

        Stock::PrimaryKeyType id;

        try
        {
            stkMapper.insert(stk);

            id = stk.getValueOfId();

            const auto& images = reqJson["images"];
            if (images.isArray())
            {
                orm::Mapper<StockImage> stkImgMapper(dbClient);

                orm::Mapper<Image> imgMapper(dbClient);

                for (const auto& img : images)
                {
                    Image::PrimaryKeyType imgId;

                    auto& imgIdJson = img["id"];

                    if (imgIdJson.isUInt64())
                    {
                        // TODO: Check exists then add to ProductImage

                        imgId = imgIdJson.asUInt64();
                    }
                    else
                    {
                        const auto &imgUrl = img["url"];
                        if (!imgUrl.isString())
                        {
                            continue;
                        }

                        Image img;
                        img.setUrl(imgUrl.asString());
                        imgMapper.insert(img);

                        imgId = img.getValueOfId();
                    }

                    StockImage stkImg;
                    stkImg.setStockId(id);
                    stkImg.setImageId(imgId);
                    stkImgMapper.insert(stkImg);
                }
            }

            orm::Mapper<StockCategory> stkCatMapper(dbClient);

            for (const auto& catId : category_ids)
            {
                StockCategory stkCat;
                stkCat.setStockId(id);
                stkCat.setCategoryId(catId.asUInt64());
                stkCatMapper.insert(stkCat);
            }

            // Create transaction
            Transaction transaction;
            transaction.setDescription("Kho #{" + std::to_string(id) + "}: Nhập {"+ std::to_string(stk.getValueOfQuantity()) +"} (VND)");
            transaction.setAmount(-(stk.getValueOfQuantity() * stk.getValueOfCostPrice()));
            transaction.setCashierId(curUser.getValueOfId());
            transaction.setPaidDate(trantor::Date::fromDbStringLocal(inout_date.asString()));
            orm::Mapper<Transaction>(dbClient).insert(transaction);

            StockTransaction stkTransaction;
            stkTransaction.setStockId(id);
            stkTransaction.setTransactionId(transaction.getValueOfId());
            orm::Mapper<StockTransaction>(dbClient).insert(stkTransaction);
        }
        catch (const std::exception &e)
        {
            dbClient->rollback();

            throw e;
        }
        catch (...)
        {
            dbClient->rollback();

            throw std::runtime_error("Lỗi trong quá trình lưu dữ liệu");
        }

        auto &retData = resJson["data"];
        retData = stkMapper.findByPrimaryKey(id).toJson();
        app_helpers::stockJsonRow(dbClient, stk, retData);
    }
    catch (const orm::UnexpectedRows &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k404NotFound;
    }
    catch (const std::logic_error &e)
    {
        resMsg = e.what();

        httpRetCode = HttpStatusCode::k406NotAcceptable;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k500InternalServerError;

        resMsg = "Lỗi hệ thống";
    }

    const auto &httpRet = HttpResponse::newHttpJsonResponse(resJson);
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}

void StockCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
}
