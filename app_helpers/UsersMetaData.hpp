#pragma once

namespace app_helpers
{
    enum class UserRole
    {
        USER_BLOCKED = 0,
        USER_UNACTIVE = 10,
        USER_NORMAL = 50,

        ADMIN_MANAGER = 100,
        ADMIN_SUB_MASTER = 190,
        ADMIN_MASTER = 200,
    };
}

