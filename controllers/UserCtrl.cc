#include "UserCtrl.h"

// Add definition of your processing function here

#include "models/Users.h"

#include "app_helpers/ApiResponse.hpp"

using User = drogon_model::web_rinphone::Users;

void UserCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = app().getDbClient();
    orm::Mapper<User> usrMap(dbClient);

    app_helpers::ApiResponse apiRes;
    auto& resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    orm::Criteria cnd(User::Cols::_deleted_at, orm::CompareOperator::IsNull);

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

        const auto &usrs = usrMap
                               .orderBy(User::Cols::_created_at, orm::SortOrder::DESC)
                               .paginate(page, limit)
                               .findBy(cnd);
        auto &retData = apiRes.data();
        retData = Json::Value(Json::arrayValue);
        for (const auto &usr : usrs)
        {
        //     app_helpers::stockJsonRow(dbClient, stk, retData.append(stk.toJson()));
            retData.append(usr.toJson());
        }

        auto& resJson = apiRes.json();

        resJson["total"] = static_cast<uint>(usrMap.count(cnd));
        resJson["current_page"] = static_cast<uint>(page);
        resJson["per_page"] = static_cast<uint>(limit);
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

void UserCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{

}

void UserCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{

}

void UserCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{

}
