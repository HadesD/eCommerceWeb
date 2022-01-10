#pragma once

#include <drogon/HttpRequest.h>

namespace app_helpers
{
    namespace Auth
    {
        static constexpr auto loggedInSessionKey = "loggedInUserId";

        bool isLoggedIn(const drogon::HttpRequestPtr& req);
    }
}
