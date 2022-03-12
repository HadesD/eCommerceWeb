#include "UserCtrl.h"

// Add definition of your processing function here

#include "models/Users.h"

#include "app_helpers/UsersMetaData.hpp"
#include "app_helpers/ApiResponse.hpp"
#include "app_helpers/Utils.hpp"

using User = drogon_model::web_rinphone::Users;
using UserRole = app_helpers::UserRole;

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
            auto usrRow = usr.toJson();
            app_helpers::userJsonRow(usr, usrRow);
            retData.append(usrRow);
        }

        auto& resJson = apiRes.json();

        apiRes.appendPaginate(page, limit, usrMap.count(cnd));
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
    app_helpers::ApiResponse apiRes;
    auto &resJson = apiRes.json();
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        auto dbClient = app().getDbClient();
        const auto &usr = orm::Mapper<User>(dbClient)
                              .findByPrimaryKey(id);

        auto &retData = apiRes.data();
        retData = usr.toJson();
        app_helpers::userJsonRow(usr, retData);
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

void UserCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
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

        const auto& name = reqJson[User::Cols::_name];
        if (!name.isString())
        {
            throw std::logic_error("Chưa nhập tên");
        }

        const auto& phone = reqJson[User::Cols::_phone];
        if (!phone.isString())
        {
            throw std::logic_error("Chưa nhập SDT");
        }

        auto role = UserRole::USER_NORMAL;
        const auto& roleJson = reqJson[User::Cols::_role];
        if (roleJson.isUInt())
        {
            role = static_cast<UserRole>(roleJson.asUInt());
            switch (role)
            {
            case UserRole::ADMIN_MASTER:
            case UserRole::ADMIN_SUB_MASTER:
            case UserRole::ADMIN_MANAGER:
                // TODO: Check has permission first
                throw std::logic_error("Bạn không đủ quyền hạn");

            case UserRole::USER_BLOCKED:
            case UserRole::USER_NORMAL:
            case UserRole::USER_UNACTIVE:
                break;

            default:
                throw std::logic_error("Không tồn tại chức vụ này");
            }
        }

        User usr;
        usr.setName(name.asString());
        usr.setPhone(phone.asString());
        usr.setRole(static_cast<uint8_t>(role));

        const auto& email = reqJson[User::Cols::_email];
        if (email.isString())
        {
            usr.setEmail(email.asString());
        }
        else
        {
            usr.setEmail(drogon::utils::genRandomString(10) + "@rinphone.com");
        }

        const auto& sns_info = reqJson[User::Cols::_sns_info];
        if (sns_info.isObject())
        {
            Json::StreamWriterBuilder builder;
            builder["commentStyle"] = "None";
            builder["indentation"] = "";
            usr.setSnsInfo(Json::writeString(builder, sns_info));
        }

        const auto& passwordJson = reqJson[User::Cols::_password];
        if (passwordJson.isString())
        {
            usr.setPassword(app_helpers::bcrypt_hash(passwordJson.asString()));
        }
        else
        {
            const auto pw = drogon::utils::genRandomString(16);
            usr.setPassword(app_helpers::bcrypt_hash(pw));
        }

        const auto now = trantor::Date::now();
        usr.setCreatedAt(now);
        usr.setUpdatedAt(now);

        auto dbClient = drogon::app().getDbClient();
        drogon::orm::Mapper<User> usrMap(dbClient);
        usrMap.insert(usr);

        auto &retData = apiRes.data();
        retData = usr.toJson();
        app_helpers::userJsonRow(usr, retData);
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

void UserCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{

}
