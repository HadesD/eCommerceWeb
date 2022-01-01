#define DROGON_TEST_MAIN

#include <drogon/drogon_test.h>
#include <drogon/drogon.h>

// #include "../controllers/CsrfTokenCtrl.h"

constexpr auto hostUrl = "http://localhost:8199";

DROGON_TEST(BasicTest)
{
    using namespace drogon;

    auto client = HttpClient::newHttpClient(hostUrl);
    client->enableCookies();
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/csrf-token");

    auto resp = client->sendRequest(req);
    CHECK(resp.second->getStatusCode() == HttpStatusCode::k200OK);
    LOG_DEBUG << resp.second->getCookie("CSRF-TOKEN");
    LOG_DEBUG << resp.second->getCookie("CSRF-TOKEN1");
    LOG_DEBUG << 123213423;
}

int main(int argc, char** argv)
{
    using namespace drogon;

    std::promise<void> p1;
    std::future<void> f1 = p1.get_future();

    // Start the main loop on another thread
    std::thread thr([&]() {
        // Queues the promise to be fulfilled after starting the loop
        app().getLoop()->queueInLoop([&p1]() { p1.set_value(); });
        app().run();
    });

    // The future is only satisfied after the event loop started
    f1.get();
    int status = test::run(argc, argv);

    // Ask the event loop to shutdown and wait
    app().getLoop()->queueInLoop([]() { app().quit(); });
    thr.join();
    return status;
}
