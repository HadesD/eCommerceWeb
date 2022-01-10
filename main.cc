#include <drogon/drogon.h>

#include "controllers/CsrfTokenCtrl.h"

int main() {
    //Set HTTP listener address and port
    // drogon::app().addListener("0.0.0.0",80);
    //Load config file
    drogon::app().loadConfigFile("./config.json");

    // CSRF check
    drogon::app().registerPreRoutingAdvice(
        [](const auto &req, auto &&fcb, auto &&fccb)
        {
            switch (req->getMethod())
            {
            case HttpMethod::Post:
            case HttpMethod::Patch:
            case HttpMethod::Put:
            {
                if (!CsrfTokenCtrl::verify(req))
                {
                    //Check failed
                    auto res = drogon::HttpResponse::newHttpResponse();
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
