/**
 * [Deprecated]
 * Do gộp tính năng nhiều item vào 1 stock nên bỏ phần này
 * Trường hợp có sản phẩm lỗi, sẽ thực hiện xuất kho với giao dịch không thu tiền (amount=0)
 * Nội dung giao dịch sẽ là sản phẩm lỗi.
 */

export default {
    0: {
        title: 'Có sẵn',
        color: 'green',
    },
    1: {
        title: 'Đã bán',
        color: 'blue',
    },
    2: {
        title: 'Hỏng / Lỗi',
        color: 'red',
    },
};
