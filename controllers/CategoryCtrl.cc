#include "CategoryCtrl.h"

#include "models/Categories.h"

#include "app_helpers/ApiResponse.hpp"

using Category = drogon_model::web_rinphone::Categories;

void CategoryCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    app_helpers::ApiResponse apiRes;
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        auto dbClient = drogon::app().getDbClient();

        drogon::orm::Mapper<Category> catMap(dbClient);
        auto cats = catMap.orderBy(Category::Cols::_parent_id).orderBy(Category::Cols::_name).findAll();

        auto &retData = apiRes.data();
        retData = Json::Value(Json::arrayValue);

        for (const auto &cat : cats)
        {
            retData.append(cat.toJson());
        }
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

void CategoryCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    app_helpers::ApiResponse apiRes;
    auto& resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        // Validate
        const auto &reqJsonPtr = req->getJsonObject();
        if (!reqJsonPtr)
        {
            throw std::logic_error("Dữ liệu gửi lên không đúng");
        }

        const auto &reqJson = *reqJsonPtr;

        const auto& name = reqJson["name"];
        if (!name.isString())
        {
            throw std::logic_error("Chưa nhập tên");
        }

        const auto& slug = reqJson["slug"];
        if (!slug.isString())
        {
            throw std::logic_error("Chưa nhập đường dẫn");
        }

        const auto& desc = reqJson["description"];
        const auto& pId = reqJson["parent_id"];

        Category cat;
        cat.setName(name.asString());
        cat.setSlug(slug.asString());
        cat.setParentId(pId.isUInt64() ? pId.asUInt64() : 0);
        if (desc.isString())
        {
            cat.setDescription(desc.asString());
        }

        auto dbClient = drogon::app().getDbClient();
        drogon::orm::Mapper<Category> catMap(dbClient);
        catMap.insert(cat);

        auto &retData = apiRes.data();
        retData = cat.toJson();
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
    catch (...)
    {
        resMsg = "Liên tiếp thất bại";

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
