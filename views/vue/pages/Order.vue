<template>
    <a-typography-title :level="1">Thông tin đơn hàng</a-typography-title>
    <a-alert type="success" show-icon>
        <template #message>Đơn hàng <a-tag>#{{ $route.params.order_id }}</a-tag> đã được tiếp nhận thành công</template>
        <template #description>
            <p>Xin chúc mừng, đơn hàng với mã số <a-tag>#{{ $route.params.order_id }}</a-tag> đã được tiếp nhận.</p>
            <p>RinPhone sẽ sớm liên hệ tới Quý Khách qua một trong số những hình thức đã cung cấp.</p>
            <p>Xin cảm ơn và chúc Quý Khách một ngày tốt lành.</p>
        </template>
    </a-alert>
</template>
<script>
export default {
    setup() {
    },
};
</script>
