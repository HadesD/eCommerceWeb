#pragma once

#include <drogon/HttpRequest.h>

#include "models/Users.h"

namespace app_helpers::Auth
{
    using User = drogon_model::web_rinphone::Users;

    User::PrimaryKeyType getSessionUserId(const drogon::HttpRequestPtr &req);
    void setSessionUserId(const drogon::HttpRequestPtr &req, const User::PrimaryKeyType userId);

    bool isLoggedIn(const drogon::HttpRequestPtr &req);

    User user(const drogon::HttpRequestPtr &req, const drogon::orm::DbClientPtr &dbClient);
    User user(const drogon::HttpRequestPtr &req, const std::shared_ptr<drogon::orm::Transaction> &dbClient);

    void logout(const drogon::HttpRequestPtr &req);
}
