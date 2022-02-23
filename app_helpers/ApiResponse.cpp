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
} // namespace app_helpers
