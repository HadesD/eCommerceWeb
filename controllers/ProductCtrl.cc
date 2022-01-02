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
        static_cast<uint8_t>(ProductStatus::DRAFT));

    const auto &reqParams = req->getParameter("id");

    if (reqParams.size())
    {
    }
    else
    {
        const auto & reqCatSlug = req->getParameter("category_slug");
        if (reqCatSlug.size())
        {
            try
            {
                orm::Mapper<Category> catMap(dbClient);
                const auto catId = catMap
                                       .findOne(orm::Criteria(Category::Cols::_slug, reqCatSlug))
                                       .getValueOfId();

                std::vector<Category::PrimaryKeyType> catIds{catId};

                std::function<void(const Category::PrimaryKeyType)> findChildCategoryId;
                findChildCategoryId = [&catIds, &catMap, &findChildCategoryId](const Category::PrimaryKeyType parentId)
                {
                    try
                    {
                        auto childCats = catMap.findBy(
                            orm::Criteria(Category::Cols::_parent_id, parentId));
                        for (const auto &childCat : childCats)
                        {
                            const auto childId = childCat.getValueOfId();
                            catIds.push_back(childId);

                            findChildCategoryId(childId);
                        }
                    }
                    catch (const std::exception &e)
                    {
                        LOG_ERROR << e.what();
                    }
                };

                findChildCategoryId(catId);

                if (catIds.size())
                {
                    const auto productCats = orm::Mapper<ProductCategory>(dbClient)
                                                 .findBy(orm::Criteria(ProductCategory::Cols::_category_id, orm::CompareOperator::In, catIds));

                    std::vector<Product::PrimaryKeyType> productIds;
                    for (const auto &productCat : productCats)
                    {
                        productIds.push_back(productCat.getValueOfProductId());
                    }

                    if (productIds.size())
                    {
                        cnd = cnd && orm::Criteria(Product::Cols::_id, orm::CompareOperator::In, productIds);
                    }
                }
            }
            catch (const orm::UnexpectedRows &e)
            {
            }
            catch (const std::exception &e)
            {
                LOG_ERROR << e.what();
            }
        }

        const auto& reqPriceRange = req->getParameter("price_range");
        LOG_DEBUG << reqPriceRange;
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
    }

    Json::Value ret;

    try
    {
        size_t page;
        size_t limit = 24;

        try
        {
            page = std::stoul(req->getParameter("page"));
        }
        catch (...)
        {
            page = 1;
        }

        const auto &prds = prdMap
                               .paginate(page, limit)
                               .findBy(cnd);
        auto &retData = ret["data"];
        retData = Json::Value(Json::arrayValue);
        for (const auto &prd : prds)
        {
            auto &prdRow = retData.append(prd.toJson());
            auto &prdCatRow = prdRow[Category::tableName];
            prdCatRow = Json::Value(Json::arrayValue);
            prd.getCategory(
                dbClient,
                [&prdCatRow](auto pairRows)
                {
                    for (const auto &pairRow : pairRows)
                    {
                        prdCatRow.append(pairRow.first.toJson());
                    }
                },
                [](const auto &e)
                {
                    LOG_ERROR << e.base().what();
                });
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
