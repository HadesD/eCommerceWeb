#include <drogon/drogon.h>

#include "controllers/CsrfTokenCtrl.h"

#include "app_helpers/Auth.hpp"
#include "app_helpers/ApiResponse.hpp"

int main() {
    //Set HTTP listener address and port
    // drogon::app().addListener("0.0.0.0",80);
    //Load config file
    drogon::app().loadConfigFile("./config.json");

    drogon::app().registerPreRoutingAdvice(
        [](const auto &req, auto &&fcb, auto &&fccb)
        {
            using namespace drogon;

            switch (req->method())
            {
            case HttpMethod::Post:
            case HttpMethod::Patch:
            case HttpMethod::Put:
            {
                // CSRF check
                if (!CsrfTokenCtrl::verify(req))
                {
                    //Check failed
                    auto res = HttpResponse::newHttpResponse();
                    res->setStatusCode(k403Forbidden);
                    res->setBody("CSRF Token Mismatch");
                    fcb(res);
                    return;
                }
            }

            default:
                // Passed
                fccb();
            }
        });

    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
