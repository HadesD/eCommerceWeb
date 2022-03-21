#include "Auth.hpp"

#include <drogon/drogon.h>

namespace app_helpers::Auth
{
    constexpr auto loggedInSessionKey = "loggedInUserId";

    User::PrimaryKeyType getSessionUserId(const drogon::HttpRequestPtr &req)
    {
        return req->session()->get<User::PrimaryKeyType>(loggedInSessionKey);
    }

    void setSessionUserId(const drogon::HttpRequestPtr &req, const User::PrimaryKeyType userId)
    {
        req->session()->modify<User::PrimaryKeyType>(
            loggedInSessionKey,
            [userId](auto &uId)
            {
                uId = userId;
            });
    }

    bool isLoggedIn(const drogon::HttpRequestPtr &req)
    {
        return (getSessionUserId(req) > 0);
    }

    User user(const drogon::HttpRequestPtr &req, const drogon::orm::DbClientPtr &dbClient)
    {
        return drogon::orm::Mapper<User>(dbClient).findByPrimaryKey(getSessionUserId(req));
    }

    User user(const drogon::HttpRequestPtr &req, const std::shared_ptr<drogon::orm::Transaction> &dbClient)
    {
        return drogon::orm::Mapper<User>(dbClient).forUpdate().findByPrimaryKey(getSessionUserId(req));
    }

    void logout(const drogon::HttpRequestPtr &req)
    {
        req->session()->erase(loggedInSessionKey);
    }
}
