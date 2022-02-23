#include "ProductCtrl.h"

#include "models/Products.h"
#include "models/Images.h"
#include "models/ProductImages.h"
#include "models/Categories.h"
#include "models/ProductCategories.h"

#include "app_helpers/ApiResponse.hpp"
#include "app_helpers/ProductsMetaData.hpp"
#include "app_helpers/Utils.hpp"

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

    app_helpers::ApiResponse apiRes;
    auto &resJson = apiRes.json();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

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
        auto &retData = apiRes.data();
        retData = Json::Value(Json::arrayValue);
        for (const auto &prd : prds)
        {
            app_helpers::productJsonRow(dbClient, prd, retData.append(prd.toJson()));
        }

        resJson["total"] = static_cast<uint>(prdMap.count(cnd));
        resJson["current_page"] = static_cast<uint>(page);
        resJson["per_page"] = static_cast<uint>(limit);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);
    callback(httpRet);
}

void ProductCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
    app_helpers::ApiResponse apiRes;
    auto &resJson = apiRes.json();
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        auto dbClient = app().getDbClient();
        const auto &prd = orm::Mapper<Product>(dbClient)
                              .findByPrimaryKey(id);

        auto &retData = apiRes.data();
        retData = prd.toJson();
        app_helpers::productJsonRow(dbClient, prd, retData);
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

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}

void ProductCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
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
        const auto& reqJson = *reqJsonPtr;

        const auto& name = reqJson[Product::Cols::_name];
        if (!name.isString())
        {
            throw std::logic_error("Chưa điền tên");
        }

        const auto& price = reqJson[Product::Cols::_price];
        if (!price.isInt())
        {
            throw std::logic_error("Chưa điền giá");
        }

        const auto& status = reqJson[Product::Cols::_status];
        if (!status.isUInt())
        {
            throw std::logic_error("Chưa chọn trạng thái");
        }

        const auto &category_ids = reqJson["category_ids"];
        if (!category_ids.isArray() || (category_ids.size() < 1))
        {
            throw std::logic_error("Chưa chọn chuyên mục");
        }

        Product prd;
        prd.setName(name.asString());
        prd.setPrice(price.asInt());
        prd.setStatus(status.asUInt());

        auto dbClient = app().getDbClient()->newTransaction();
        orm::Mapper<Product> prdMapper{dbClient};

        const auto& description = reqJson[Product::Cols::_description];
        if (description.isString())
        {
            prd.setDescription(description.asString());
        }

        const auto& detail = reqJson[Product::Cols::_detail];
        if (detail.isString())
        {
            prd.setDetail(detail.asString());
        }

        const auto& specification = reqJson[Product::Cols::_specification];
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

            const auto& images = reqJson[Image::tableName];
            if (images.isArray())
            {
                orm::Mapper<ProductImage> prdImgMapper(dbClient);

                orm::Mapper<Image> imgMapper(dbClient);

                for (const auto& img : images)
                {
                    Image::PrimaryKeyType imgId;

                    auto& imgIdJson = img[Image::Cols::_id];

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

                    ProductImage prdImg;
                    prdImg.setProductId(id);
                    prdImg.setImageId(imgId);
                    prdImgMapper.insert(prdImg);
                }
            }

            orm::Mapper<ProductCategory> prdCatMapper(dbClient);

            for (const auto& catId : category_ids)
            {
                ProductCategory prdCat;
                prdCat.setProductId(id);
                prdCat.setCategoryId(catId.asUInt64());
                prdCatMapper.insert(prdCat);
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

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}

void ProductCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
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
        const auto& reqJson = *reqJsonPtr;

        const auto &category_ids = reqJson["category_ids"];
        if (!category_ids.isNull() && (!category_ids.isArray() || category_ids.size() < 1))
        {
            throw std::logic_error("Chưa chọn chuyên mục");
        }

        auto dbClient = app().getDbClient()->newTransaction();
        orm::Mapper<Product> prdMapper{dbClient};
        auto prd = prdMapper
                       .forUpdate()
                       .findByPrimaryKey(id);

        const auto& name = reqJson[Product::Cols::_name];
        if (!name.isNull() && name.isString())
        {
            prd.setName(name.asString());
        }

        const auto& price = reqJson[Product::Cols::_price];
        if (!price.isNull() && price.isInt())
        {
            prd.setPrice(price.asInt());
        }

        const auto& status = reqJson[Product::Cols::_status];
        if (!status.isNull() && status.isUInt())
        {
            prd.setStatus(status.asUInt());
        }

        const auto& description = reqJson[Product::Cols::_description];
        if (!description.isNull() && description.isString())
        {
            prd.setDescription(description.asString());
        }

        const auto& detail = reqJson[Product::Cols::_detail];
        if (!detail.isNull() && detail.isString())
        {
            prd.setDetail(detail.asString());
        }

        const auto& specification = reqJson[Product::Cols::_specification];
        if (!specification.isNull() && specification.isString())
        {
            prd.setSpecification(specification.asString());
        }

        const auto now = trantor::Date::now();

        prd.setUpdatedAt(now);

        try
        {
            prdMapper.update(prd);

            const auto& images = reqJson[Image::tableName];
            if (images.isArray() && (images.size() > 0))
            {
                orm::Mapper<ProductImage> prdImgMapper(dbClient);

                orm::Mapper<Image> imgMapper(dbClient);

                std::vector<Image::PrimaryKeyType> imgIds;
                imgIds.reserve(images.size());

                for (const auto& img : images)
                {
                    Image::PrimaryKeyType imgId;

                    auto& imgIdJson = img[Image::Cols::_id];

                    bool needToInsertPI = false;

                    if (imgIdJson.isUInt64())
                    {
                        imgId = imgIdJson.asUInt64();

                        if (!prdImgMapper.count(orm::Criteria(ProductImage::Cols::_image_id, imgId) && orm::Criteria(ProductImage::Cols::_product_id, id)))
                        {
                            needToInsertPI = true;
                        }
                    }
                    else
                    {
                        const auto &imgUrl = img[Image::Cols::_url];
                        if (!imgUrl.isString())
                        {
                            continue;
                        }

                        Image img;
                        img.setUrl(app_helpers::trim(imgUrl.asString()));
                        imgMapper.insert(img);

                        imgId = img.getValueOfId();

                        needToInsertPI = true;
                    }

                    if (needToInsertPI)
                    {
                        ProductImage prdImg;
                        prdImg.setProductId(id);
                        prdImg.setImageId(imgId);
                        prdImgMapper.insert(prdImg);
                    }

                    imgIds.push_back(imgId);
                }

                if (imgIds.size())
                {
                    prdImgMapper.deleteBy(orm::Criteria(ProductImage::Cols::_product_id, id) &&
                                          orm::Criteria(ProductImage::Cols::_image_id, orm::CompareOperator::NotIn, imgIds));
                }
            }

            orm::Mapper<ProductCategory> prdCatMapper(dbClient);
            prdCatMapper.deleteBy(orm::Criteria(ProductCategory::Cols::_product_id, id));

            for (const auto &catId : category_ids)
            {
                ProductCategory prdCat;
                prdCat.setProductId(id);
                prdCat.setCategoryId(catId.asUInt64());
                prdCatMapper.insert(prdCat);
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

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
