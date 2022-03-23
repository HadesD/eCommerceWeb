#include "DashboardCtrl.h"

#include "app_helpers/ApiResponse.hpp"

void DashboardCtrl::statistics(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) const
{
    app_helpers::ApiResponse apiRes;
    auto &resMsg = apiRes.message();
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;


    const auto &httpRet = HttpResponse::newHttpJsonResponse(apiRes.toJson());
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
