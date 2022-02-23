#include "models/Users.h"

namespace app_helpers
{
    void userJsonRow(const drogon_model::web_rinphone::Users& usr, Json::Value& usrRow)
    {
        using User = drogon_model::web_rinphone::Users;

        usrRow.removeMember(User::Cols::_password);

        auto& sns_info = usrRow[User::Cols::_sns_info];
        if (sns_info.isString())
        {
            Json::CharReaderBuilder builder;
            builder["collectComments"] = false;
            JSONCPP_STRING errs;
            std::stringstream ss(sns_info.asString());
            bool ok = Json::parseFromStream(builder, ss, &sns_info, &errs);
        }
    }
}

