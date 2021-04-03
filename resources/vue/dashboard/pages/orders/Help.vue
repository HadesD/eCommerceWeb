<template>
    <div>
1. Tạo hoá đơn khi Bán hàng:
- Truy cập trang Tạo Đơn
- Điền thông tin đơn hàng:
	- Thông tin sản phẩm muốn mua
		- Số lượng muốn mua (để quản lí)
		- Hình thức trả tiền (để quản lí)
		- Các [Mặt hàng xuất kho]
			- Các giao dịch ứng với từng mặt hàng
- Điền các [Giao dịch thêm] (nếu có):
	- Phí ship: Số âm nếu mình phải trả, =0 nếu khách trả
	- Chiết khấu/giảm giá: Số âm

2. Thu hồi sản phẩm / trả hàng
- Truy cập vào Đơn hàng lúc trước (ở 1.)
- Trong mục [Xuất kho] của hàng khách muốn trả:
    - Chuyển trạng thái từ [Đã bán] thành [Đã thu hồi]
    - Thêm giao dịch
        - Nội dung: Thu hồi máy
        - Số tiền: (Số âm) số tiền trả lại đến tay khách
        - Ngày thanh toán: Ngày trả lại số tiền

- Xoá [Mặt hàng xuất kho] mà khách trả

3. Phí sinh hoạt / Tiền lương / ...

- Truy cập vào trang Tạo Đơn
- ...
    </div>
</template>
