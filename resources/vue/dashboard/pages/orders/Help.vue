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
- Thêm giao dịch ở [Giao dịch thêm]:
	- Giao dịch khoản 1:
	* Đối với trường hợp khách trả góp, số giao dịch thêm vào khoản 1 sẽ bằng số giao dịch khách trả góp
		- Nội dung: Mua sản phẩm XX
		- Số tiền: =Tổng số tiền tiệm đã nhận từ khách
		- Ngày thanh toán: Ngày ứng với giao dịch lúc trước khách trả

	- Giao dịch khoản 2:
		- Nội dung: Trả sản phẩm XXX (imei/tên)
		- Số tiền: =Tổng số tiền phải trả khách
		- Ngày thanh toán: Ngày trả tiền

- Xoá [Mặt hàng xuất kho] mà khách trả

3. Phí sinh hoạt / Tiền lương / ...

- Truy cập vào trang Tạo Đơn
- ...
    </div>
</template>
