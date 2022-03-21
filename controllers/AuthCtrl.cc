#include "AuthCtrl.h"

#include "models/Users.h"

#include "app_helpers/ApiResponse.hpp"
#include "app_helpers/Auth.hpp"
#include "app_helpers/UsersMetaData.hpp"
#include "app_helpers/Utils.hpp"

using User = drogon_model::web_rinphone::Users;

Task<> AuthCtrl::login(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
{
    app_helpers::ApiResponse apiRes;
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        const auto &reqJsonPtr = req->getJsonObject();
        if (!reqJsonPtr)
        {
            throw std::logic_error("Dữ liệu gửi lên không đúng");
        }

        const auto &reqJson = *reqJsonPtr;

        const auto& emailJson = reqJson[User::Cols::_email];
        if (!emailJson.isString())
        {
            throw std::logic_error("Chưa nhập email");
        }

        const auto& passwordJson = reqJson[User::Cols::_password];
        if (!passwordJson.isString())
        {
            throw std::logic_error("Chưa nhập mật khẩu");
        }

        auto dbClient = drogon::app().getDbClient();
        const auto &user = orm::Mapper<User>(dbClient)
                               .findOne(
                                   orm::Criteria(User::Cols::_deleted_at, orm::CompareOperator::IsNull) &&
                                   orm::Criteria(User::Cols::_email, emailJson.asString()));

        if (!app_helpers::bcrypt_verify(passwordJson.asString(), user.getValueOfPassword()))
        {
            throw std::logic_error("Email hoặc mật khẩu không đúng");
        }

        app_helpers::Auth::setSessionUserId(req, user.getValueOfId());

        auto userJson = user.toJson();

        app_helpers::userJsonRow(user, userJson);

        apiRes.data() = userJson;
    }
    catch (const orm::UnexpectedRows &e)
    {
        resMsg = "Email hoặc mật khẩu không đúng";

        httpRetCode = HttpStatusCode::k401Unauthorized;
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

    auto httpRes = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRes->setStatusCode(httpRetCode);
    callback(httpRes);

    co_return;
}

Task<> AuthCtrl::logout(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
{
    app_helpers::Auth::logout(req);

    callback(HttpResponse::newHttpResponse());

    co_return;
}

Task<> AuthCtrl::user(const HttpRequestPtr req, std::function<void(const HttpResponsePtr &)> callback)
{
    app_helpers::ApiResponse apiRes;
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        auto dbClient = drogon::app().getDbClient();
        const auto &user = orm::Mapper<User>(dbClient)
                               .findOne(
                                   orm::Criteria(User::Cols::_deleted_at, orm::CompareOperator::IsNull) &&
                                   orm::Criteria(User::Cols::_id, app_helpers::Auth::getSessionUserId(req)));

        apiRes.data() = user.toJson();
    }
    catch (const orm::UnexpectedRows &e)
    {
        resMsg = "Chưa đăng nhập";

        httpRetCode = HttpStatusCode::k401Unauthorized;
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

    auto httpRes = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRes->setStatusCode(httpRetCode);
    callback(httpRes);

    co_return;
}
