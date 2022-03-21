/**
 *
 *  AuthFilter.cc
 *
 */

#include "AuthFilter.h"

#include <drogon/drogon.h>

#include "app_helpers/Auth.hpp"
#include "app_helpers/UsersMetaData.hpp"

using namespace drogon;
using namespace app_helpers;

void AuthFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    auto willBePass = Auth::isLoggedIn(req);
    if (willBePass)
    {
        auto dbClient = drogon::app().getDbClient();
        try
        {
            auto usr = Auth::user(req, dbClient);

            willBePass = hasRole(usr, UserRole::ADMIN_MANAGER);
        }
        catch(const orm::UnexpectedRows& e)
        {
            willBePass = false;
        }

    }

    if (!willBePass)
    {
        Auth::logout(req);

        auto res = drogon::HttpResponse::newHttpResponse();
        res->setStatusCode(k401Unauthorized);
        fcb(res);

        return;
    }

    fccb();
}
