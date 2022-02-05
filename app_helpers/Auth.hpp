#pragma once

#include <drogon/HttpRequest.h>

#include "models/Users.h"

namespace app_helpers
{
    namespace Auth
    {
        using User = drogon_model::web_rinphone::Users;

        bool login(const std::string& email, const std::string& password, const bool remember = true);

        void login(const User::PrimaryKeyType id);

        bool isLoggedIn(const drogon::HttpRequestPtr& req);

        User user(const drogon::HttpRequestPtr& req);

        void logout(const drogon::HttpRequestPtr& req);
    }
}