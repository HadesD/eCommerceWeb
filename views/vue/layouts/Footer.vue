<template>
    <a-layout-footer style="background: #FFF;">
        <a-row type="flex">
            <a-col :flex="2">
                <div style="font-size:28px;color: blueviolet;"><img src="/favicon.ico" alt="RinPhone.vn logo" />inPhone.vn</div>
                <a-typography-title :level="4">Bạn có thắc mắc cần giải đáp? 24/7 HotLine</a-typography-title>
                <a-typography-paragraph>
                    <ul>
                        <li><a-typography-title :level="5">Đường dây nóng 1: <a href="tel:+84774475777">077-447-5777</a></a-typography-title></li>
                        <li><a-typography-title :level="5">Đường dây nóng 2: <a href="tel:+84867575277">086-757-5277</a></a-typography-title></li>
                    </ul>
                </a-typography-paragraph>
                <a-typography-text type="secondary"><a href="https://goo.gl/maps/N1kXZFf5G77ZiV3q9" target="_blank"><EnvironmentFilled /> ĐT637, Vĩnh Hảo, Vĩnh Thạnh, Bình Định</a></a-typography-text>
                <div style="margin: 10px 0;">
                    <a-space>
                        <a href="https://fb.me/rinphonevn" target="_blank">
                            <a-button type="primary" shape="circle">
                                <template #icon>
                                    <FacebookFilled />
                                </template>
                            </a-button>
                        </a>
                        <a href="">
                            <a-button type="primary" danger shape="circle">
                                <template #icon>
                                    <YoutubeFilled />
                                </template>
                            </a-button>
                        </a>
                    </a-space>
                </div>
            </a-col>
            <a-col :flex="3">
                <a-row type="flex">
                    <a-col :flex="2">
                        <a-space direction="vertical">
                            <a-typography-title :level="5">Chuyên mục</a-typography-title>
                            <a-typography-text>Điện thoại</a-typography-text>
                            <a-typography-text>Phụ kiện</a-typography-text>
                        </a-space>
                    </a-col>
                    <a-col :flex="2">
                        <a-space direction="vertical">
                            <a-typography-title :level="5">Chăm sóc Khách Hàng</a-typography-title>
                            <a-typography-text>Quản lý tài khoản</a-typography-text>
                            <a-typography-text>Theo dõi đơn hàng</a-typography-text>
                            <a-typography-text>Dịch vụ Đổi / Trả</a-typography-text>
                            <a-typography-text>Các câu hỏi thường gặp</a-typography-text>
                        </a-space>
                    </a-col>
                </a-row>
            </a-col>
        </a-row>
    </a-layout-footer>
    <a-layout-footer style="background-color:#fafafa;">
        <a-row type="flex">
            <a-col :flex="3">Copyright <CopyrightOutlined /> 2021 RinPhone.vn. All rights reserved.</a-col>
            <a-col :flex="2" style="text-align: right;">
                <a-space>
                    <CreditCardTwoTone style="font-size: 25px;" />
                    <CreditCardTwoTone style="font-size: 25px;" />
                </a-space>
            </a-col>
        </a-row>
    </a-layout-footer>
</template>
<script>
import {
    EnvironmentFilled, FacebookFilled, YoutubeFilled,
    CopyrightOutlined, CreditCardTwoTone,
    LeftCircleOutlined, RightCircleOutlined,
} from '@ant-design/icons-vue';

export default {
    components: {
        EnvironmentFilled, FacebookFilled, YoutubeFilled,
        CopyrightOutlined, CreditCardTwoTone,
        LeftCircleOutlined, RightCircleOutlined,
    },
}
</script>
