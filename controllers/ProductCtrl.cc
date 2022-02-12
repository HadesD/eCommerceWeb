#include "ProductCtrl.h"

#include "models/Products.h"
#include "models/Categories.h"
#include "models/ProductCategories.h"

#include "app_helpers/ProductsMetaData.hpp"

using Product = drogon_model::web_rinphone::Products;
using Category = drogon_model::web_rinphone::Categories;
using ProductCategory = drogon_model::web_rinphone::ProductCategories;

// add definition of your processing function here

void ProductCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = app().getDbClient();
    orm::Mapper<Product> prdMap(dbClient);

    orm::Criteria cnd(
        Product::Cols::_status,
        orm::CompareOperator::NE,
        static_cast<uint8_t>(app_helpers::ProductStatus::DRAFT));
    cnd = cnd && orm::Criteria(Product::Cols::_deleted_at, orm::CompareOperator::IsNull);

    orm::SortOrder orderSort{orm::SortOrder::DESC};
    auto orderBy = Product::Cols::_created_at;

    const auto &reqIds = req->getParameter("ids");

    if (reqIds.size())
    {
        std::vector<Product::PrimaryKeyType> prdIds;
        std::istringstream iss(reqIds);
        std::string idStr;
        while (std::getline(iss, idStr, ','))
        {
            try
            {
                prdIds.push_back(std::stoul(idStr));
            }
            catch (...)
            {
            }
        }
        cnd = cnd && orm::Criteria(Product::Cols::_id, orm::CompareOperator::In, prdIds);
    }
    else
    {
        Category::PrimaryKeyType catId = 0;
        orm::Mapper<Category> catMap(dbClient);
        const auto& reqCatId = req->getParameter("category_id");
        if (reqCatId.size())
        {
            try
            {
                catId = std::stoul(reqCatId);
            }
            catch(...)
            {
            }
        }
        else
        {
            const auto &reqCatSlug = req->getParameter("category_slug");
            if (reqCatSlug.size())
            {
                try
                {
                    catId = catMap
                                .findOne(orm::Criteria(Category::Cols::_slug, reqCatSlug))
                                .getValueOfId();
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

                std::vector<Product::PrimaryKeyType> productIds;
                const auto productCats = orm::Mapper<ProductCategory>(dbClient)
                                             .findBy(orm::Criteria(ProductCategory::Cols::_category_id, orm::CompareOperator::In, catIds));

                for (const auto &productCat : productCats)
                {
                    productIds.push_back(productCat.getValueOfProductId());
                }
                if (!productIds.size())
                {
                    productIds.push_back(0);
                }

                cnd = cnd && orm::Criteria(Product::Cols::_id, orm::CompareOperator::In, productIds);
            }
            catch (const orm::UnexpectedRows &e)
            {
            }
            catch (const std::exception &e)
            {
                LOG_ERROR << e.what();
            }
        }

        // Price range: low,high
        const auto &reqPriceRange = req->getParameter("price_range");
        if (reqPriceRange.size())
        {
            auto pos = reqPriceRange.find(',');

            if (pos != std::string::npos)
            {
                try
                {
                    auto from = std::stol(reqPriceRange.substr(0, pos));
                    auto to = std::stol(reqPriceRange.substr(pos + 1));
                    cnd = cnd &&
                          orm::Criteria(Product::Cols::_price, orm::CompareOperator::GE, from) &&
                          orm::Criteria(Product::Cols::_price, orm::CompareOperator::LE, to);
                }
                catch (...)
                {
                }
            }
        }

        // Search
        const auto &reqKw = req->getParameter("keyword");
        if (reqKw.size())
        {
            cnd = cnd && orm::Criteria(Product::Cols::_name, orm::CompareOperator::Like, '%' + reqKw + '%');
        }

        // Sort
        const auto &reqSortBy = req->getParameter("sort_by");
        if (reqSortBy.size() >= 2)
        {
            const auto &tmpSortBy = reqSortBy.substr(1);
            static std::vector<std::string> allowCols{
                Product::Cols::_created_at,
                Product::Cols::_price,
                Product::Cols::_name,
            };
            if (std::find(allowCols.cbegin(), allowCols.cend(), tmpSortBy) != allowCols.cend())
            {
                orderSort = (reqSortBy[0] == '+') ? orm::SortOrder::ASC : orm::SortOrder::DESC;
                orderBy = tmpSortBy;
            }
        }
    }

    Json::Value ret;

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

        const auto &prds = prdMap
                               .orderBy(orderBy, orderSort)
                               .paginate(page, limit)
                               .findBy(cnd);
        auto &retData = ret["data"];
        retData = Json::Value(Json::arrayValue);
        for (const auto &prd : prds)
        {
            app_helpers::productJsonRow(dbClient, prd, retData.append(prd.toJson()));
        }

        ret["total"] = static_cast<uint>(prdMap.count(cnd));
        ret["current_page"] = static_cast<uint>(page);
        ret["per_page"] = static_cast<uint>(limit);

        callback(HttpResponse::newHttpJsonResponse(ret));
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        auto res = HttpResponse::newHttpResponse();
        res->setStatusCode(HttpStatusCode::k500InternalServerError);

        callback(res);
    }
}

void ProductCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
    try
    {
        auto dbClient = app().getDbClient();
        const auto &prd = orm::Mapper<Product>(dbClient)
                              .findByPrimaryKey(id);

        Json::Value ret;
        auto &retData = ret["data"];
        retData = prd.toJson();
        app_helpers::productJsonRow(dbClient, prd, retData);

        callback(HttpResponse::newHttpJsonResponse(ret));
    }
    catch (const orm::UnexpectedRows &e)
    {
        LOG_ERROR << e.what();

        callback(HttpResponse::newNotFoundResponse());
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        auto res = HttpResponse::newHttpResponse();
        res->setStatusCode(HttpStatusCode::k500InternalServerError);

        callback(res);
    }
}

void ProductCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
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
        orm::Mapper<Product> prdMapper{dbClient};
        auto prd = prdMapper
                       .forUpdate()
                       .findByPrimaryKey(id);

        const auto& name = reqJson["name"];
        if (name.isString())
        {
            prd.setName(name.asString());
        }

        const auto& price = reqJson["price"];
        if (price.isInt())
        {
            prd.setPrice(price.asInt());
        }

        const auto& description = reqJson["description"];
        if (description.isString())
        {
            prd.setDescription(description.asString());
        }

        const auto& detail = reqJson["detail"];
        if (detail.isString())
        {
            prd.setDetail(detail.asString());
        }

        const auto& specification = reqJson["specification"];
        if (specification.isString())
        {
            prd.setSpecification(specification.asString());
        }

        const auto& status = reqJson["status"];
        if (status.isUInt())
        {
            prd.setStatus(status.asUInt());
        }

        // Update
        prdMapper.update(prd);

        auto &retData = resJson["data"];
        retData = prd.toJson();
        app_helpers::productJsonRow(dbClient, prd, retData);
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
