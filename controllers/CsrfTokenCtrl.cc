#include "CsrfTokenCtrl.h"

static constexpr auto csrfTokenSessionKey = "csrf_token";
static constexpr auto csrfCookieKey = "CSRF-TOKEN";
static constexpr auto csrfHeaderKey = "X-CSRF-TOKEN";

void CsrfTokenCtrl::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::srand(0);

    const auto token = drogon::utils::getMd5(std::to_string(std::rand()));
    req->session()->modify<std::string>(
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
