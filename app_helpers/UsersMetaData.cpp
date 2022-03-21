#include "UsersMetaData.hpp"

#include "Utils.hpp"

namespace app_helpers
{
    void userJsonRow(const drogon_model::web_rinphone::Users& usr, Json::Value& usrRow)
    {
        using User = drogon_model::web_rinphone::Users;

        usrRow.removeMember(User::Cols::_password);
        usrRow.removeMember(User::Cols::_remember_token);

        auto& sns_info = usrRow[User::Cols::_sns_info];
        if (sns_info.isString())
        {
            sns_info = json_decode(sns_info.asString());
        }
    }

    bool hasRole(const drogon_model::web_rinphone::Users& usr, const UserRole role)
    {
        return usr.getValueOfRole() >= static_cast<uint8_t>(role);
    }
}

