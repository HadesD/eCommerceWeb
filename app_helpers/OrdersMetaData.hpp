#pragma once

namespace app_helpers
{
    enum class OrderStatus
    {
        PROCESSING = 0,
        CANCELED = 10,
        PAID = 50,
        PAYING = 51,
        SHIPPING = 100,
        COMPLETED = 200,
    };

    enum class PaymentMethod
    {
        ONCE = 1,
        INSTALMENT = 2, // Tra gop
    };
}
