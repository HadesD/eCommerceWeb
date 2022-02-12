#include "ProductCtrl.h"

#include "models/Products.h"
#include "models/Categories.h"
#include "models/ProductCategories.h"

#include "app_helpers/ProductsMetaData.hpp"

using Product = drogon_model::web_rinphone::Products;
using Category = drogon_model::web_rinphone::Categories;
using ProductCategory = drogon_model::web_rinphone::ProductCategories;
using Image = drogon_model::web_rinphone::Images;
using ProductImage = drogon_model::web_rinphone::ProductImages;

// add definition of your processing function here

void ProductCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = app().getDbClient();
    orm::Mapper<Product> prdMap(dbClient);

    orm::Criteria cnd(Product::Cols::_deleted_at, orm::CompareOperator::IsNull);

    if (!req->parameters().count("is_dashboard"))
    {
        cnd = cnd && orm::Criteria(Product::Cols::_status,
                                   orm::CompareOperator::NE,
                                   static_cast<uint8_t>(app_helpers::ProductStatus::DRAFT));
    }

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

void ProductCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
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
        Product prd;

        const auto& name = reqJson["name"];
        if (!name.isString())
        {
            throw std::logic_error("Chưa điền tên");
        }
        prd.setName(name.asString());

        const auto& price = reqJson["price"];
        if (!price.isInt())
        {
            throw std::logic_error("Chưa điền giá");
        }
        prd.setPrice(price.asInt());

        const auto& status = reqJson["status"];
        if (!status.isUInt())
        {
            throw std::logic_error("Chưa chọn trạng thái");
        }
        prd.setStatus(status.asUInt());

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

        const auto now = trantor::Date::now();

        prd.setCreatedAt(now);
        prd.setUpdatedAt(now);

        Product::PrimaryKeyType id;

        try
        {
            prdMapper.insert(prd);

            id = prd.getValueOfId();

            const auto& images = reqJson["images"];
            if (images.isArray())
            {
                orm::Mapper<ProductImage> prdImgMapper(dbClient);

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

                    ProductImage prdImg;
                    prdImg.setProductId(id);
                    prdImg.setImageId(imgId);
                    prdImgMapper.insert(prdImg);
                }
            }

            const auto& category_ids = reqJson["category_ids"];
            if (category_ids.isArray())
            {
                orm::Mapper<ProductCategory> prdCatMapper(dbClient);

                for (const auto& catId : category_ids)
                {
                    ProductCategory prdCat;
                    prdCat.setProductId(id);
                    prdCat.setCategoryId(catId.asUInt64());
                    prdCatMapper.insert(prdCat);
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

        auto &retData = resJson["data"];
        retData = prdMapper.findByPrimaryKey(id).toJson();
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

        const auto& status = reqJson["status"];
        if (status.isUInt())
        {
            prd.setStatus(status.asUInt());
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

        const auto now = trantor::Date::now();

        prd.setUpdatedAt(now);

        try
        {
            prdMapper.update(prd);

            const auto& images = reqJson["images"];
            if (images.isArray())
            {
                orm::Mapper<ProductImage> prdImgMapper(dbClient);

                orm::Mapper<Image> imgMapper(dbClient);

                orm::Criteria delPrdImgCnd(ProductImage::Cols::_product_id, id);

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

                        ProductImage prdImg;
                        prdImg.setProductId(id);
                        prdImg.setImageId(imgId);
                        prdImgMapper.insert(prdImg);
                    }

                    delPrdImgCnd = delPrdImgCnd && orm::Criteria(ProductImage::Cols::_image_id, orm::CompareOperator::NE, imgId);
                }

                // TODO: Change to NOT IN
                prdImgMapper.deleteBy(delPrdImgCnd);
            }

            const auto& category_ids = reqJson["category_ids"];
            if (category_ids.isArray())
            {
                orm::Criteria delPrdCatCnd(ProductCategory::Cols::_product_id, id);

                orm::Mapper<ProductCategory> prdCatMapper(dbClient);
                prdCatMapper.deleteBy(delPrdCatCnd);

                for (const auto& catId : category_ids)
                {
                    ProductCategory prdCat;
                    prdCat.setProductId(id);
                    prdCat.setCategoryId(catId.asUInt64());
                    prdCatMapper.insert(prdCat);
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

        auto &retData = resJson["data"];
        retData = prdMapper.findByPrimaryKey(id).toJson();
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
