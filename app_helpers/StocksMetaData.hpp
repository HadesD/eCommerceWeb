#pragma once

#include "models/Stocks.h"

namespace app_helpers
{
    void stockJsonRow(const drogon::orm::DbClientPtr &dbClient, const drogon_model::web_rinphone::Stocks& stk, Json::Value &stkRow);
}
