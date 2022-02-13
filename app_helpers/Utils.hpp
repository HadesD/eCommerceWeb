#pragma once

#include <string>

namespace app_helpers
{
    constexpr std::string_view WHITESPACE = " \n\r\t\f\v";

    std::string ltrim(const std::string &s)
    {
        size_t start = s.find_first_not_of(WHITESPACE);
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    std::string& ltrim(std::string &s)
    {
        s.erase(0, s.find_first_not_of(WHITESPACE));
        return s;
    }

    std::string rtrim(const std::string &s)
    {
        size_t end = s.find_last_not_of(WHITESPACE);
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    }

    std::string& rtrim(std::string &s)
    {
        s.erase(s.find_last_not_of(WHITESPACE) + 1);
        return s;
    }

    std::string trim(const std::string &s)
    {
        return rtrim(ltrim(s));
    }

    std::string& trim(std::string &s)
    {
        LOG_DEBUG << 1;
        return rtrim(ltrim(s));
    }
}
