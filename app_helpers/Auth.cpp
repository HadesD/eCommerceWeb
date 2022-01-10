#include "Auth.hpp"

namespace app_helpers
{
    namespace Auth
    {
        bool isLoggedIn(const drogon::HttpRequestPtr& req)
        {
            return (req->session()->get<uint64_t>(loggedInSessionKey) > 0);
        }
    }
}
