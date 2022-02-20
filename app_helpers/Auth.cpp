#include "Auth.hpp"

#include <drogon/drogon.h>

namespace app_helpers
{
    namespace Auth
    {
        constexpr auto loggedInSessionKey = "loggedInUserId";

        inline User::PrimaryKeyType getSessionUserId(const drogon::HttpRequestPtr &req)
        {
            return req->session()->get<User::PrimaryKeyType>(loggedInSessionKey);
        }

        inline void setSessionUserId(const drogon::HttpRequestPtr& req, const User::PrimaryKeyType userId)
        {
            req->session()->modify<User::PrimaryKeyType>(
                loggedInSessionKey,
                [userId](auto &uId)
                {
                    uId = userId;
                });
        }

        bool login(const std::string& email, const std::string& password, const bool remember)
        {
            using namespace drogon;

            try
            {
                auto dbClient = app().getDbClient();
                const auto& user = orm::Mapper<User>(dbClient)
                    .findBy(
                        orm::Criteria(User::Cols::_deleted_at, orm::CompareOperator::IsNull) &&
                        orm::Criteria(User::Cols::_email, email)
                    );

                return true;
            }
            catch (const orm::UnexpectedRows&)
            {
            }
            catch (const std::exception &e)
            {
                LOG_ERROR << e.what();
            }
            catch (...)
            {
            }

            return false;
        }

        void login(const User::PrimaryKeyType id)
        {
        }

        bool isLoggedIn(const drogon::HttpRequestPtr& req)
        {
            return (getSessionUserId(req) > 0);
        }

        User user(const drogon::HttpRequestPtr& req, const drogon::orm::DbClientPtr dbClient)
        {
            // auto dbClient = drogon::app().getDbClient();

            return drogon::orm::Mapper<User>(dbClient).findByPrimaryKey(getSessionUserId(req));
        }

        void logout(const drogon::HttpRequestPtr& req)
        {
            req->session()->erase(loggedInSessionKey);
        }
    }
}
