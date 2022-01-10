<template>
    <a-layout-footer style="background: #FFF;">
        <a-row type="flex">
            <a-col :flex="2">
                <div>
                    <img src="/logo-white.png" alt="RinPhone.vn logo" style="padding: 5px 10px; background-color: blueviolet;border-radius:5px;" />
                </div>
                <a-typography-title :level="4">Bạn có thắc mắc cần giải đáp? 24/7 HotLine</a-typography-title>
                <a-typography-paragraph>
                    <ul>
                        <li><a-typography-title :level="5">Đường dây nóng: <PhoneOutlined /> <a href="tel:+84774475777">077-447-5777</a></a-typography-title></li>
                        <li><a-typography-title :level="5">Đường dây nóng: <PhoneOutlined /> <a href="tel:+84867575277">086-757-5277</a></a-typography-title></li>
                    </ul>
                </a-typography-paragraph>
                <a-typography-text type="secondary"><a href="https://goo.gl/maps/N1kXZFf5G77ZiV3q9" target="_blank" title="Xem trên Google Maps"><EnvironmentFilled /> Khu phố Định An, thị trấn Vĩnh Thạnh, Bình Định</a></a-typography-text>
                <div style="margin: 10px 0;">
                    <a-space>
                        <a href="https://fb.com/rinphonevn" target="_blank">
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
                            <a-typography-title :level="5">Apple</a-typography-title>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'iphone'}}">iPhone</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'airpods'}}">AirPods</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'ipad'}}">iPad</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'apple-watch'}}">Apple Watch</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'apple'}}">Xem thêm <DoubleRightOutlined /></router-link></a-typography-text>
                        </a-space>
                    </a-col>
                    <a-col :flex="2">
                        <a-space direction="vertical">
                            <a-typography-title :level="5">Máy Android / Khác</a-typography-title>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'samsung'}}">Samsung</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'oppo'}}">OPPO</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'vivo'}}">Vivo</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'dien-thoai-re'}}">Điện thoại giá rẻ</router-link></a-typography-text>
                        </a-space>
                    </a-col>
                    <a-col :flex="2">
                        <a-space direction="vertical">
                            <a-typography-title :level="5">Chuyên mục khác</a-typography-title>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'phu-kien'}}">Phụ kiện</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'linh-kien'}}">Linh kiện</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'category',params:{category_slug:'may-tinh'}}">Máy tính</router-link></a-typography-text>
                        </a-space>
                    </a-col>
                    <a-col :flex="2">
                        <a-space direction="vertical">
                            <a-typography-title :level="5">Hỗ trợ Khách Hàng</a-typography-title>
                            <a-typography-text><router-link :to="{name:'faq',params:{faq_slug:'chinh-sach-bao-mat-thong-tin'}}">Chính sách bảo mật thông tin</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'faq',params:{faq_slug:'chinh-sach-doi-tra'}}">Chính sách Đổi / Trả</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'faq',params:{faq_slug:'chinh-sach-van-chuyen'}}">Chính sách vận chuyển</router-link></a-typography-text>
                            <a-typography-text><router-link :to="{name:'faq',params:{faq_slug:'cac-cau-hoi-thuong-gap'}}">Các câu hỏi thường gặp</router-link></a-typography-text>
                        </a-space>
                    </a-col>
                </a-row>
            </a-col>
        </a-row>
    </a-layout-footer>
    <a-layout-footer style="background-color:#fafafa;">
        <a-row type="flex">
            <a-col :flex="3">Copyright <CopyrightOutlined /> {{ dayjs().year() }} RinPhone. All rights reserved.</a-col>
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
    LeftCircleOutlined, RightCircleOutlined, PhoneOutlined,
    DoubleRightOutlined,
} from '@ant-design/icons-vue';
import dayjs from 'dayjs';

export default {
    components: {
        EnvironmentFilled, FacebookFilled, YoutubeFilled,
        CopyrightOutlined, CreditCardTwoTone,
        LeftCircleOutlined, RightCircleOutlined, PhoneOutlined,
        DoubleRightOutlined,
    },
    setup() {
        return {
            dayjs,
        };
    },
}
</script>
