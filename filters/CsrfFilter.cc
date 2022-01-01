/**
 *
 *  CsrfFilter.cc
 *
 */

#include "CsrfFilter.h"

#include "controllers/CsrfTokenCtrl.h"

using namespace drogon;

void CsrfFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    // Edit your logic here
    if (CsrfTokenCtrl::verify(req))
    {
        // Passed
        fccb();
        return;
    }
    //Check failed
    auto res = drogon::HttpResponse::newHttpResponse();
    res->setStatusCode(k401Unauthorized);
    res->setBody("CSRF Token Mismatch");
    fcb(res);
}
