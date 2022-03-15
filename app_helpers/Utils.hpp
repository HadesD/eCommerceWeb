#pragma once

#include <string>
#include <drogon/orm/Mapper.h>

namespace app_helpers
{
    std::string ltrim(const std::string &s);
    std::string& ltrim(std::string &s);
    std::string rtrim(const std::string &s);
    std::string& rtrim(std::string &s);
    std::string trim(const std::string &s);
    std::string& trim(std::string &s);

    std::string bcrypt_hash(const std::string& txt);
    bool bcrypt_verify(const std::string& txt, const std::string& hash);

    std::string json_encode(const Json::Value& json);
    Json::Value json_decode(const std::string& jsonStr);
}
