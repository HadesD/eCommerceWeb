#include "ApiResponse.hpp"

namespace app_helpers
{
    ApiResponse::ApiResponse() : data_(json_["data"]), message_(json_["message"])
    {
    }

    const Json::Value &ApiResponse::toJson() const
    {
        return json_;
    }

    Json::Value &ApiResponse::json()
    {
        return json_;
    }

    Json::Value &ApiResponse::data()
    {
        return data_;
    }

    Json::Value &ApiResponse::message()
    {
        return message_;
    }

    void ApiResponse::appendPaginate(const size_t page, const size_t limit, const size_t total)
    {
        json_["total"] = static_cast<uint>(total);
        json_["current_page"] = static_cast<uint>(page);
        json_["per_page"] = static_cast<uint>(limit);
    }
} // namespace app_helpers
