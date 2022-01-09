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
    switch (req->getMethod())
    {
        case HttpMethod::Post:
        case HttpMethod::Patch:
        case HttpMethod::Put:
        {
            if (CsrfTokenCtrl::verify(req))
            {
                // Passed
                fccb();
                return;
            }
        }
        break;

        default:
            return;
    }
    //Check failed
    auto res = drogon::HttpResponse::newHttpResponse();
    res->setStatusCode(k403Forbidden);
    res->setBody("CSRF Token Mismatch");
    fcb(res);
}
