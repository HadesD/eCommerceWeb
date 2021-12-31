#include "CsrfTokenCtrl.h"

constexpr auto csrfTokenSessionKey = "csrf_token";
constexpr auto csrfCookieKey = "XSRF-TOKEN";
constexpr auto csrfHeaderKey = "X-XSRF-TOKEN";

void CsrfTokenCtrl::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::string token = "";
    req->session()->modify<decltype(token)>(
        csrfTokenSessionKey,
        [token](auto &curToken)
        {
            curToken = token;
        });

    auto res = HttpResponse::newHttpResponse();

    Cookie tokenCookie(csrfCookieKey, token);
    tokenCookie.setExpiresDate(trantor::Date::now().after(300));
    tokenCookie.setHttpOnly(true);
    tokenCookie.setSecure(true);

    res->addCookie(tokenCookie);

    callback(res);
}

bool CsrfTokenCtrl::verify(const HttpRequestPtr& req)
{
    return req->session()->get<std::string>(csrfTokenSessionKey) == req->getHeader(csrfHeaderKey);
}
