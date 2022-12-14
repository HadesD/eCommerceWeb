#include "UserCtrl.h"

// Add definition of your processing function here

#include "models/Users.h"

#include "app_helpers/ApiResponse.hpp"
#include "app_helpers/ApiRequestParse.hpp"
#include "app_helpers/Auth.hpp"
#include "app_helpers/UsersMetaData.hpp"
#include "app_helpers/Utils.hpp"

using User = drogon_model::web_rinphone::Users;
using UserRole = app_helpers::UserRole;

using namespace app_helpers;

Task<> UserCtrl::get(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
{
    auto dbClient = app().getDbClient();

    app_helpers::ApiResponse apiRes;
    auto& resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    orm::Criteria cnd(User::Cols::_deleted_at, orm::CompareOperator::IsNull);

    try
    {
        size_t page = getPage(req);
        size_t limit = 24;

        orm::Mapper<User> usrMap(dbClient);
        const auto &usrs = usrMap
                               .orderBy(User::Cols::_role, orm::SortOrder::DESC)
                               .orderBy(User::Cols::_created_at, orm::SortOrder::DESC)
                               .paginate(page, limit)
                               .findBy(cnd);
        auto &retData = apiRes.data();
        retData = Json::Value(Json::arrayValue);
        for (const auto &usr : usrs)
        {
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

    co_return;
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
                              .findOne(orm::Criteria(User::Cols::_id, id) &&
                                       orm::Criteria(User::Cols::_deleted_at, orm::CompareOperator::IsNull));

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
            throw std::logic_error("D??? li???u g???i l??n kh??ng ????ng");
        }

        const auto &reqJson = *reqJsonPtr;

        const auto& name = reqJson[User::Cols::_name];
        if (!name.isString())
        {
            throw std::logic_error("Ch??a nh???p t??n");
        }

        const auto& phone = reqJson[User::Cols::_phone];
        if (!phone.isString())
        {
            throw std::logic_error("Ch??a nh???p SDT");
        }

        auto dbClient = drogon::app().getDbClient();

        auto role = UserRole::USER_NORMAL;
        const auto& roleJson = reqJson[User::Cols::_role];
        if (roleJson.isUInt())
        {
            const auto authUser = app_helpers::Auth::user(req, dbClient);

            role = static_cast<UserRole>(roleJson.asUInt());
            switch (role)
            {
            case UserRole::ADMIN_MASTER:
            case UserRole::ADMIN_SUB_MASTER:
            {
                if (!app_helpers::hasRole(authUser, UserRole::ADMIN_MASTER))
                {
                    throw std::logic_error("B???n kh??ng ????? quy???n h???n");
                }

                break;
            }
            case UserRole::ADMIN_MANAGER:
            {
                if (!app_helpers::hasRole(authUser, UserRole::ADMIN_SUB_MASTER))
                {
                    throw std::logic_error("B???n kh??ng ????? quy???n h???n");
                }

                break;
            }

            case UserRole::USER_BLOCKED:
            case UserRole::USER_NORMAL:
            case UserRole::USER_UNACTIVE:
                break;

            default:
                throw std::logic_error("Kh??ng t???n t???i ch???c v??? n??y");
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
            usr.setSnsInfo(app_helpers::json_encode(sns_info));
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

        resMsg = "L???i h??? th???ng";
    }
    catch (...)
    {
        resMsg = "Li??n ti???p th???t b???i";

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}

void UserCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
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
            throw std::logic_error("D??? li???u g???i l??n kh??ng ????ng");
        }

        auto dbClient = drogon::app().getDbClient()->newTransaction();
        drogon::orm::Mapper<User> usrMap(dbClient);

        User usr = usrMap.forUpdate().findByPrimaryKey(id);

        const auto &reqJson = *reqJsonPtr;

        const auto& name = reqJson[User::Cols::_name];
        if (!name.isNull())
        {
            if (!name.isString())
            {
                throw std::logic_error("T??n kh??ng ????ng");
            }

            usr.setName(name.asString());
        }

        const auto& phone = reqJson[User::Cols::_phone];
        if (!phone.isNull())
        {
            if (!phone.isString())
            {
                throw std::logic_error("S??? ??i???n tho???i kh??ng ????ng");
            }

            usr.setPhone(phone.asString());
        }

        const auto authUser = app_helpers::Auth::user(req, dbClient);

        const auto userRole = static_cast<UserRole>(usr.getValueOfRole());

        const auto& roleJson = reqJson[User::Cols::_role];
        if (roleJson.isUInt())
        {
            if (!app_helpers::hasRole(authUser, userRole))
            {
                throw std::logic_error("B???n kh??ng ????? quy???n h???n");
            }

            auto role = static_cast<UserRole>(roleJson.asUInt());

            switch (role)
            {
            case UserRole::ADMIN_MASTER:
            case UserRole::ADMIN_SUB_MASTER:
            {
                if (!app_helpers::hasRole(authUser, UserRole::ADMIN_MASTER))
                {
                    throw std::logic_error("B???n kh??ng ????? quy???n h???n");
                }

                break;
            }
            case UserRole::ADMIN_MANAGER:
            {
                if (!app_helpers::hasRole(authUser, UserRole::ADMIN_SUB_MASTER))
                {
                    throw std::logic_error("B???n kh??ng ????? quy???n h???n");
                }

                break;
            }

            case UserRole::USER_BLOCKED:
            case UserRole::USER_NORMAL:
            case UserRole::USER_UNACTIVE:
                break;

            default:
                throw std::logic_error("Kh??ng t???n t???i ch???c v??? n??y");
            }

            usr.setRole(static_cast<uint8_t>(role));
        }

        const auto& email = reqJson[User::Cols::_email];
        if (email.isString())
        {
            usr.setEmail(email.asString());
        }

        const auto& sns_info = reqJson[User::Cols::_sns_info];
        if (sns_info.isObject())
        {
            usr.setSnsInfo(app_helpers::json_encode(sns_info));
        }

        const auto& passwordJson = reqJson[User::Cols::_password];
        if (passwordJson.isString())
        {
            if (!app_helpers::hasRole(authUser, userRole))
            {
                throw std::logic_error("B???n kh??ng ????? quy???n h???n");
            }

            usr.setPassword(app_helpers::bcrypt_hash(passwordJson.asString()));
        }

        const auto now = trantor::Date::now();
        usr.setUpdatedAt(now);

        usrMap.update(usr);

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

        resMsg = "L???i h??? th???ng";
    }
    catch (...)
    {
        resMsg = "Li??n ti???p th???t b???i";

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
