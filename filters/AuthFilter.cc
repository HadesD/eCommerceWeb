/**
 *
 *  AuthFilter.cc
 *
 */

#include "AuthFilter.h"

#include "app_helpers/Auth.hpp"

using namespace drogon;
using namespace app_helpers;

void AuthFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    if (Auth::isLoggedIn(req))
    {
        fccb();
        return;
    }

    auto res = drogon::HttpResponse::newHttpResponse();
    res->setStatusCode(k401Unauthorized);
    fcb(res);
}
