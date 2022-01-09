#include <drogon/drogon.h>
#include "filters/CsrfFilter.h"

int main() {
    //Set HTTP listener address and port
    // drogon::app().addListener("0.0.0.0",80);
    //Load config file
    drogon::app().loadConfigFile("./config.json");

    // drogon::app().registerPreHandlingAdvice([](auto req){
    //     LOG_DEBUG << 11;
    // });
    // drogon::app().registerFilter(std::make_shared<CsrfFilter>());

    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
