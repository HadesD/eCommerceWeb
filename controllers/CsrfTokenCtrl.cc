#include "CsrfTokenCtrl.h"

static constexpr auto csrfTokenSessionKey = "csrf_token";
static constexpr auto csrfCookieKey = "CSRF-TOKEN";
static constexpr auto csrfHeaderKey = "X-CSRF-TOKEN";

void CsrfTokenCtrl::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    const auto& token = drogon::utils::genRandomString(35);
    req->session()->modify<std::string>(
        csrfTokenSessionKey,
        [&token](auto &curToken)
        {
            curToken = token;
        });

    auto res = HttpResponse::newHttpResponse();

    Cookie tokenCookie(csrfCookieKey, token);
    tokenCookie.setExpiresDate(trantor::Date::now().after(300));
    tokenCookie.setHttpOnly(true);
    tokenCookie.setSecure(req->isOnSecureConnection());

    res->addCookie(tokenCookie);

    callback(res);
}

bool CsrfTokenCtrl::verify(const HttpRequestPtr& req)
{
    const auto& token = req->session()->get<std::string>(csrfTokenSessionKey);
    return token.size() && ((token == req->getCookie(csrfCookieKey)) || (token == req->getHeader(csrfHeaderKey)));
}
