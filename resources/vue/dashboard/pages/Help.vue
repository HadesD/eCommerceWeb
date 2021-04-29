<template>
    <a-page-header
        title="Hướng dẫn quản lí các loại Hoá đơn / Giao dịch trong tiệm"
        sub-title="Cập nhật: 2021/04/13"
    />
    <a-collapse :activeKey="['/orders/new']">
        <a-collapse-panel key="/orders/new" header="Tạo hoá đơn khi Bán hàng">
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
        </a-collapse-panel>
        <a-collapse-panel key="/refunds" header="Thu hồi sản phẩm / trả hàng">
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
        </a-collapse-panel>
        <a-collapse-panel key="/use-stock-fix-stock" header="Xuất Linh Kiện: Dùng khi Sửa máy nhập kho">
            <h3>B1. Truy cập vào Kho Linh kiện, mở cửa sổ sửa thông tin linh kiện cần xuất</h3>
            <ul>
                <li>1. Mục [Số Lượng]: Giảm số lượng linh kiện cần dùng khi sửa máy</li>
                <li>2. Mục [Ngày xuất / nhập]: Chọn đúng ngày xuất linh kiện khỏi kho</li>
                <li>3. Lưu lại kho</li>
            </ul>
            <h3>B2. Truy cập vào Kho máy, mở cửa sổ thông tin máy cần sửa</h3>
            <ul>
                <li>1. Tạo thêm [Giao Dịch Thêm]</li>
                <li>2. Nội dung: Điền nội dung sửa máy (VD: Thay pin...)</li>
                <li>3. Số tiền: (Số âm) bằng số tiền của linh kiện xuất khỏi kho</li>
                <li>4. Mục [Ngày]: Chọn đúng ngày xuất linh kiện khỏi kho</li>
                <li>5. Lưu lại kho</li>
            </ul>
        </a-collapse-panel>
        <a-collapse-panel key="/use-stock-fix-for-customer" header="Xuất Linh Kiện: Dùng khi sửa máy cho khách hàng">
            <h3>(TH: Khách mua linh kiện) Truy cập vào trang Hoá đơn </h3>
            <ul>
                <li>**0. Mục [Khách hàng] Đối với khách hàng đến thay trực tiếp ở quán, Khách của hoá đơn sẽ là tên thợ sửa</li>
                <li>1. Mục [Xuất kho] sẽ xuất linh kiện sử dụng để thay / sửa cho khách</li>
                <li>2. Thêm [Giao dịch] ứng với linh kiện vừa xuất kho. Số tiên sẽ = 1.2 lần giá trị linh kiện</li>
                <li>3. Thợ sửa trả tiền cho tiệm rồi thu ngân lưu hoá đơn lại</li>
                <li>**. Chi phí sửa cho khách, thợ sửa sẽ được nhận 100% mà không phải đưa lại quán</li>
            </ul>
            <h3>(TH: Khách bảo trì máy lỗi) Truy cập vào trang Hoá đơn của máy khách muốn bảo trì</h3>
            <ul>
                <li>1. Mục [Xuất kho] sẽ xuất linh kiện sử dụng để thay / sửa cho khách</li>
                <li>2. Thêm [Giao dịch] ứng với linh kiện vừa xuất kho. Số tiên sẽ = 0 hoặc tuỳ vào thời gian quá hạn bảo trì mà lấy tiền</li>
            </ul>
        </a-collapse-panel>
        <a-collapse-panel key="/fee-salary" header="Phí sinh hoạt / Tiền lương / ...">
            <h3>Truy cập vào trang Tạo Đơn</h3>
            <ul>
                <li>- ...</li>
            </ul>
        </a-collapse-panel>
    </a-collapse>
</template>

<script>
export default {
}
</script>
