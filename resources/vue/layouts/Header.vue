<template>
    <a-layout-header style="background-color: #FFF;">
        <router-link to="/">
            <div style="float: left;padding: 0 5px;">
                <img src="/favicon.ico" />
            </div>
        </router-link>
        <a-menu mode="horizontal">
            <a-menu-item key="1" style="font-size: 20px;">
                <a href="https://m.me/rinphonevn" target="_blank">
                    <FacebookFilled style="font-size: 20px;" /> @rinphonevn
                </a>
            </a-menu-item>
            <a-menu-item key="2" style="font-size: 20px;">
                <a href="tel:+84774475777">
                    <PhoneFilled style="font-size: 20px;" /> 077-447-5777
                </a>
            </a-menu-item>
        </a-menu>
        <a-menu mode="horizontal" style="position: absolute; right: 0; top: 0; min-width: 245px;">
            <a-menu-item key="1">
                <UserOutlined /> Đăng ký
            </a-menu-item>
            <a-menu-item key="2">
                <LockFilled /> Đăng nhập
            </a-menu-item>
        </a-menu>
    </a-layout-header>
</template>
<script>
import {
    FacebookFilled, PhoneFilled,
    UserOutlined, LockFilled,
} from '@ant-design/icons-vue';

export default {
    components: {
        FacebookFilled, PhoneFilled,
        UserOutlined, LockFilled,
    },
}
</script>
