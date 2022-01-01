#include "ProductCtrl.h"

#include "models/Products.h"
#include "models/Categories.h"

#include "app_helpers/ProductsMetaData.hpp"

using Product = drogon_model::web_rinphone::Products;
using Category = drogon_model::web_rinphone::Categories;

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
        try
        {
            orm::Mapper<Category> catMap(dbClient);
            const auto catId = catMap
                .findOne(orm::Criteria(Category::Cols::_slug, req->getParameter("category_slug")))
                .getValueOfId();

            std::vector<Category::PrimaryKeyType> catIds{catId};

            auto findChildCategoryId = [&catIds, &catMap](Category::PrimaryKeyType parentId) {
                try
                {
                }
                catch(const std::exception& e)
                {
                    LOG_ERROR << e.what();
                }

            };
        }
        catch (const orm::UnexpectedRows &e)
        {
        }
        catch (const std::exception &e)
        {
            LOG_ERROR << e.what();
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

        const auto& prds = prdMap
            .paginate(page, limit)
            .findBy(cnd);
        auto& retData = ret["data"];
        retData = Json::Value(Json::arrayValue);
        for (const auto& prd : prds)
        {
            retData.append(prd.toJson());
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
