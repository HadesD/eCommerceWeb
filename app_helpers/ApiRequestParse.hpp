#pragma once

#include <drogon/HttpController.h>

namespace app_helpers
{
    size_t getPage(const drogon::HttpRequestPtr &req);
} // namespace app_helpers

