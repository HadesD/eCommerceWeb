#include "CategoryCtrl.h"

#include "models/Categories.h"

// add definition of your processing function here

using Category = drogon_model::web_rinphone::Categories;

void CategoryCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    Json::Value ret;

    try
    {
        auto dbClient = drogon::app().getDbClient();

        drogon::orm::Mapper<Category> catMap(dbClient);
        auto cats = catMap.orderBy(Category::Cols::_parent_id).orderBy(Category::Cols::_name).findAll();

        auto &retData = ret["data"];
        retData = Json::Value(Json::arrayValue);

        for (const auto &cat : cats)
        {
            retData.append(cat.toJson());
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        auto res = HttpResponse::newHttpResponse();
        res->setStatusCode(HttpStatusCode::k500InternalServerError);

        callback(res);

        return;
    }

    callback(HttpResponse::newHttpJsonResponse(ret));
}

void CategoryCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    Json::Value resJson;
    auto& resMsg = resJson["message"];
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

        auto &retData = resJson["data"];
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

    const auto &httpRet = HttpResponse::newHttpJsonResponse(resJson);
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
