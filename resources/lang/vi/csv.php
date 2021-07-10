<?php

use App\Models\Order;

return [
    'id' => '#ID',
    'created_at' => 'Tạo lúc',
    'updated_at' => 'Cập nhật lúc',
    'deleted_at' => 'Xoá lúc',
    'type' => 'Loại',
    'status' => 'Trạng thái',

    # Order
    'deal_date' => 'Ngày chốt đơn',
    'stock_cost_total' => 'Tổng tiền nhâp kho',
    'need_paid_total' => 'Tổng cần trả',
    'stock_earned_total' => 'Tổng tiền đã thu (kho)',
    'remaining_need_paid_total' => 'Tổng tiền còn nợ',
    'addon_transaction_total' => 'Tổng tiền GD thêm',
    '_order_status' => [
        Order::STS_PROCESSING => 'Đang xử lý',
        Order::STS_CANCELED => 'Đã bị huỷ',
        Order::STS_PAID => 'Đã thanh toán',
        Order::STS_PAYING => 'Đang thanh toán',
        Order::STS_SHIPPING => 'Đang ship',
        Order::STS_COMPLETED => 'Hoàn tất',
    ],

    # Stock
    'cost_price' => 'Giá nhập',
];
