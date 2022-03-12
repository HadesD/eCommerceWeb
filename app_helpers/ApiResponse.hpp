#pragma once

#include <json/json.h>

namespace app_helpers
{
    class ApiResponse
    {
        public:
            ApiResponse();

            const Json::Value& toJson() const;
            Json::Value& json();
            Json::Value& data();
            Json::Value& message();
            void appendPaginate(const size_t page, const size_t limit, const size_t total);

        private:
            Json::Value json_;
            Json::Value& data_;
            Json::Value& message_;
    };
} // namespace app_helpers
