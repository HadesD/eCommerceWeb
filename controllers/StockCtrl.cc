#include "StockCtrl.h"
// add definition of your processing function here

#include "models/Stocks.h"
#include "models/Categories.h"
#include "models/StockCategories.h"
#include "models/Images.h"
#include "models/StockImages.h"
#include "models/Transactions.h"
#include "models/StockTransactions.h"

#include "app_helpers/ApiResponse.hpp"
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

    app_helpers::ApiResponse apiRes;
    auto &resJson = apiRes.json();
    auto &resMsg = apiRes.message();
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
        auto &retData = apiRes.data();
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
    app_helpers::ApiResponse apiRes;
    auto &resJson = apiRes.json();
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        auto dbClient = app().getDbClient();
        const auto &stk = orm::Mapper<Stock>(dbClient)
                              .findByPrimaryKey(id);

        auto &retData = apiRes.data();
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

    auto res = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    res->setStatusCode(httpRetCode);

    callback(res);
}

void StockCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    app_helpers::ApiResponse apiRes;
    auto &resJson = apiRes.json();
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        const auto reqJsonPtr = req->getJsonObject();
        if (!reqJsonPtr)
        {
            throw std::logic_error("Dữ liệu gửi lên không đúng");
        }
        const auto &reqJson = *reqJsonPtr;

        Stock stk;

        const auto &name = reqJson[Stock::Cols::_name];
        if (!name.isString())
        {
            throw std::logic_error("Chưa điền tên");
        }
        stk.setName(name.asString());

        const auto &idi = reqJson[Stock::Cols::_idi];
        if (!idi.isString())
        {
            throw std::logic_error("Chưa điền idi");
        }
        stk.setIdi(idi.asString());

        const auto &sell_price = reqJson[Stock::Cols::_sell_price];
        if (!sell_price.isInt())
        {
            throw std::logic_error("Chưa điền giá");
        }
        stk.setSellPrice(sell_price.asInt());

        const auto &cost_price = reqJson[Stock::Cols::_cost_price];
        if (!cost_price.isInt() || (cost_price.asInt() < 0))
        {
            throw std::logic_error("Chưa điền giá");
        }
        stk.setCostPrice(cost_price.asInt());

        const auto &quantity = reqJson[Stock::Cols::_quantity];
        if (!quantity.isInt() || (quantity.asInt() < 1))
        {
            throw std::logic_error("Chưa chọn số lượng");
        }
        stk.setQuantity(quantity.asInt());

        const auto &tester_id = reqJson[Stock::Cols::_tester_id];
        if (!tester_id.isUInt64())
        {
            throw std::logic_error("Chưa chọn người test");
        }
        stk.setTesterId(quantity.asUInt64());

        const auto &category_ids = reqJson["category_ids"];
        if (!category_ids.isArray() || (category_ids.size() < 1))
        {
            throw std::logic_error("Chưa chọn chuyên mục");
        }

        const auto &inout_date = reqJson["inout_date"];
        if (!inout_date.isString())
        {
            throw std::logic_error("Chưa chọn ngày nhập / trả");
        }

        const auto &note = reqJson[Stock::Cols::_note];
        if (note.isString())
        {
            stk.setNote(note.asString());
        }

        auto dbClient = app().getDbClient()->newTransaction();
        orm::Mapper<Stock> stkMapper{dbClient};

        // TODO: Fix updated user
        // const auto& curUser = app_helpers::Auth::user(req, dbClient);
        // stk.setUpdatedUserId(curUser.getValueOfId());
        stk.setUpdatedUserId(1);

        const auto now = trantor::Date::now();

        stk.setCreatedAt(now);
        stk.setUpdatedAt(now);

        Stock::PrimaryKeyType id;

        try
        {
            stkMapper.insert(stk);

            id = stk.getValueOfId();

            const auto &images = reqJson[Image::tableName];
            if (images.isArray())
            {
                orm::Mapper<StockImage> stkImgMapper(dbClient);

                orm::Mapper<Image> imgMapper(dbClient);

                for (const auto &img : images)
                {
                    Image::PrimaryKeyType imgId;

                    auto &imgIdJson = img[Image::Cols::_id];

                    if (imgIdJson.isUInt64())
                    {
                        // TODO: Check exists then add to ProductImage

                        imgId = imgIdJson.asUInt64();
                    }
                    else
                    {
                        const auto &imgUrl = img[Image::Cols::_id];
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

            for (const auto &catId : category_ids)
            {
                StockCategory stkCat;
                stkCat.setStockId(id);
                stkCat.setCategoryId(catId.asUInt64());
                stkCatMapper.insert(stkCat);
            }

            // Create transaction
            Transaction transaction;
            transaction.setDescription("Kho #{" + std::to_string(id) + "}: Nhập [" + std::to_string(stk.getValueOfQuantity()) + "] cái (VND)");
            transaction.setAmount(-(stk.getValueOfQuantity() * stk.getValueOfCostPrice()));
            // TODO: Fix updated user
            // transaction.setCashierId(curUser.getValueOfId());
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

        auto &retData = apiRes.data();
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

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}

void StockCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
    app_helpers::ApiResponse apiRes;
    auto &resJson = apiRes.json();
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        const auto reqJsonPtr = req->getJsonObject();
        if (!reqJsonPtr)
        {
            throw std::logic_error("Dữ liệu gửi lên không đúng");
        }
        const auto &reqJson = *reqJsonPtr;

        auto dbClient = app().getDbClient()->newTransaction();
        orm::Mapper<Stock> stkMapper{dbClient};

        Stock stk = stkMapper.forUpdate().findByPrimaryKey(id);
        auto prevQuantity = stk.getValueOfQuantity();

        const auto &name = reqJson[Stock::Cols::_name];
        if (!name.isNull())
        {
            if (name.isString() && (name.asString().size() > 0))
            {
                stk.setName(name.asString());
            }
            else
            {
                throw std::logic_error("Chưa điền tên");
            }
        }

        const auto &idi = reqJson[Stock::Cols::_idi];
        if (!idi.isNull())
        {
            if (idi.isString() && (idi.asString().size() > 0))
            {
                stk.setIdi(idi.asString());
            }
        }

        const auto &sell_price = reqJson[Stock::Cols::_sell_price];
        if (!sell_price.isNull())
        {
            if (sell_price.isInt() && (sell_price.asInt() > 0))
            {
                stk.setSellPrice(sell_price.asInt());
            }
        }

        // // TODO: only master admin
        //! cost_price must not change-able
        // const auto& cost_price = reqJson["cost_price"];
        // if (!cost_price.isNull() && cost_price.isInt() && (cost_price.asInt() >= 0))
        // {
        //     stk.setCostPrice(cost_price.asInt());
        // }

        const auto &quantity = reqJson[Stock::Cols::_quantity];
        if (quantity.isInt() && (quantity.asInt() >= 0))
        {
            stk.setQuantity(quantity.asInt());
        }

        // TODO: only master admin
        const auto &tester_id = reqJson[Stock::Cols::_tester_id];
        if (!tester_id.isNull())
        {
            if (tester_id.isUInt64())
            {
                stk.setTesterId(quantity.asUInt64());
            }
            else
            {
                throw std::logic_error("Chưa chọn người test");
            }
        }

        const auto &category_ids = reqJson["category_ids"];
        if (!category_ids.isArray() || (category_ids.size() < 1))
        {
            throw std::logic_error("Chưa chọn chuyên mục");
        }

        auto cmpQuantity = prevQuantity - stk.getValueOfQuantity();

        const auto &inout_date = reqJson["inout_date"];
        if (cmpQuantity && !inout_date.isString())
        {
            throw std::logic_error("Chưa chọn ngày nhập / trả");
        }

        const auto &note = reqJson[Stock::Cols::_note];
        if (note.isString())
        {
            stk.setNote(note.asString());
        }

        // TODO: Fix updated user
        // const auto& curUser = app_helpers::Auth::user(req, dbClient);
        // stk.setUpdatedUserId(curUser.getValueOfId());
        stk.setUpdatedUserId(1);

        const auto now = trantor::Date::now();

        stk.setUpdatedAt(now);

        try
        {
            stkMapper.update(stk);

            const auto &images = reqJson[Image::tableName];
            if (images.isArray())
            {
                orm::Mapper<StockImage> stkImgMapper(dbClient);

                orm::Mapper<Image> imgMapper(dbClient);

                std::vector<Image::PrimaryKeyType> imgIds;
                imgIds.reserve(images.size());

                for (const auto &img : images)
                {
                    Image::PrimaryKeyType imgId;

                    auto &imgIdJson = img[Image::Cols::_id];

                    if (imgIdJson.isUInt64())
                    {
                        // TODO: Check exists then add to ProductImage

                        imgId = imgIdJson.asUInt64();
                    }
                    else
                    {
                        const auto &imgUrl = img[Image::Cols::_url];
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

                    imgIds.push_back(imgId);
                }

                if (imgIds.size())
                {
                    stkImgMapper.deleteBy(orm::Criteria(StockImage::Cols::_stock_id, id) &&
                                          orm::Criteria(StockImage::Cols::_image_id, orm::CompareOperator::NotIn, imgIds));
                }
            }

            orm::Mapper<StockCategory> stkCatMapper(dbClient);
            stkCatMapper.deleteBy(orm::Criteria(StockCategory::Cols::_stock_id, id));

            for (const auto &catId : category_ids)
            {
                StockCategory stkCat;
                stkCat.setStockId(id);
                stkCat.setCategoryId(catId.asUInt64());
                stkCatMapper.insert(stkCat);
            }

            // Create transaction
            orm::Mapper<Transaction> txnMapper(dbClient);
            orm::Mapper<StockTransaction> stkTxnMapper(dbClient);
            if (cmpQuantity)
            {
                Transaction transaction;
                transaction.setDescription("Kho #{" + std::to_string(id) + "}: " + ((cmpQuantity < 0) ? "Nhập" : "Xuất") + " [" + std::to_string(std::abs(cmpQuantity)) + "] cái (VND)");
                transaction.setAmount(-(stk.getValueOfQuantity() * stk.getValueOfCostPrice()));
                // TODO: Fix updated user
                // transaction.setCashierId(curUser.getValueOfId());
                transaction.setPaidDate(trantor::Date::fromDbStringLocal(inout_date.asString()));
                txnMapper.insert(transaction);

                StockTransaction stkTransaction;
                stkTransaction.setStockId(id);
                stkTransaction.setTransactionId(transaction.getValueOfId());
                stkTxnMapper.insert(stkTransaction);
            }

            const auto &transactions = reqJson[Transaction::tableName];
            if (transactions.isArray())
            {
                for (const auto &_txn : transactions)
                {
                    if (!_txn.isObject())
                    {
                        continue;
                    }

                    const auto &txnIdJson = _txn[Transaction::Cols::_id];
                    if (!txnIdJson.isUInt64())
                    {
                        continue;
                    }
                    const auto txnId = txnIdJson.asUInt64();

                    const auto &paid_date = _txn[Transaction::Cols::_paid_date];
                    if (!paid_date.isString())
                    {
                        continue;
                    }

                    stkTxnMapper.findOne(orm::Criteria(StockTransaction::Cols::_stock_id, id) && orm::Criteria(StockTransaction::Cols::_transaction_id, txnId));

                    auto txn = txnMapper.forUpdate().findByPrimaryKey(txnId);
                    txn.setPaidDate(trantor::Date::fromDbStringLocal(paid_date.asString()));
                    txnMapper.update(txn);
                }
            }
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

        auto &retData = apiRes.data();
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

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
