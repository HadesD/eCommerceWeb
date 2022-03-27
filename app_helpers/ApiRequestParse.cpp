#include "ApiRequestParse.hpp"

namespace app_helpers
{
    size_t getPage(const drogon::HttpRequestPtr &req)
    {
        size_t page;

        try
        {
            page = std::stoul(req->getParameter("page"));
            page = (page < 1) ? 1 : page;
        }
        catch (...)
        {
            page = 1;
        }

        return page;
    }
} // namespace app_helpers
