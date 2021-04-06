<template>
    <div>
        <a-page-header
            title="Hướng dẫn quản lí các loại Hoá đơn / Giao dịch trong tiệm"
            sub-title="Cập nhật: 2021/04/06"
        />
        <h2>1. Tạo hoá đơn khi Bán hàng</h2>
        <h3>Truy cập trang <router-link to="/orders/new">Tạo đơn</router-link></h3>
        <ul>
            <li>
                <span>Điền thông tin đơn hàng</span>
                <ul>
                    <li>
                        <span>Thông tin sản phẩm muốn mua</span>
                        <ul>
                            <li>Số lượng muốn mua (để quản lí)</li>
                            <li>Hình thức trả tiền (để quản lí)</li>
                            <li>
                                <span>Các [Mặt hàng xuất kho]</span>
                                <ul>
                                    <li>Các giao dịch ứng với từng mặt hàng</li>
                                </ul>
                            </li>
                        </ul>
                    </li>
                </ul>
            </li>
            <li>
                <span>Điền các [Giao dịch thêm] (nếu có)</span>
                <ul>
                    <li>Phí ship: Số âm nếu mình phải trả, =0 nếu khách trả</li>
                    <li>Chiết khấu/giảm giá: Số âm</li>
                </ul>
            </li>
        </ul>

        <h2>2. Thu hồi sản phẩm / trả hàng</h2>
        <h3>Truy cập vào Đơn hàng lúc trước (ở 1.)</h3>
        <ul>
            <li>
                <span>Trong mục [Xuất kho] của hàng khách muốn trả</span>
                <ul>
                    <li>Chuyển trạng thái từ [Đã bán] thành [Đã thu hồi]</li>
                    <li>
                        <span>Thêm giao dịch</span>
                        <ul>
                            <li>Nội dung: Thu hồi máy</li>
                            <li>Số tiền: (Số âm) số tiền trả lại đến tay khách</li>
                            <li>Ngày thanh toán: Ngày trả lại số tiền</li>
                        </ul>
                    </li>
                </ul>
            </li>
        </ul>

        <h2>3. Phí sinh hoạt / Tiền lương / ...</h2>
        <h3>Truy cập vào trang Tạo Đơn</h3>
        - ...
    </div>
</template>
