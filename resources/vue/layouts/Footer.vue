<template>
    <a-layout-footer>
        <a-row type="flex">
            <a-col :flex="2">
                <div style="font-size:28px;"><img src="/favicon.ico" />inPhone.vn</div>
                <a-typography-title :level="4">Bạn có thắc mắc cần giải đáp? 24/7 HotLine</a-typography-title>
                <a-typography-paragraph>
                    <ul>
                        <li><a-typography-title :level="5">Đường dây nóng 1: <a href="tel:+84774475777">077-447-5777</a></a-typography-title></li>
                        <li><a-typography-title :level="5">Đường dây nóng 2: <a href="tel:+84867575277">086-757-5277</a></a-typography-title></li>
                    </ul>
                </a-typography-paragraph>
                <a-typography-text type="secondary"><a href="https://goo.gl/maps/N1kXZFf5G77ZiV3q9" target="_blank"><AimOutlined /> ĐT637, Vĩnh Hảo, Vĩnh Thạnh, Bình Định</a></a-typography-text>
                <a-divider />
                <a-space>
                    <a href="https://fb.me/rinphonevn">
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
            </a-col>
            <a-col :flex="3">3 / 5</a-col>
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
    AimOutlined, FacebookFilled, YoutubeFilled,
    CopyrightOutlined, CreditCardTwoTone,
} from '@ant-design/icons-vue';

export default {
    components: {
        AimOutlined, FacebookFilled, YoutubeFilled,
        CopyrightOutlined, CreditCardTwoTone,
    },
}
</script>
