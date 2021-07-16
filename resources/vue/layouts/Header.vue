<template>
    <a-layout-header style="background-color: #FFF;">
        <router-link to="/">
            <div style="float: left;padding: 0 5px;">
                <img src="/favicon.ico" />
            </div>
        </router-link>
        <a-menu mode="horizontal">
            <a-menu-item style="font-size: 20px;">
                <a href="https://m.me/rinphonevn" target="_blank">
                    <MessageOutlined style="font-size: 20px;" /> fb.me/rinphonevn
                </a>
            </a-menu-item>
            <a-menu-item style="font-size: 20px;">
                <a href="tel:+84">
                    <PhoneOutlined style="font-size: 20px;" /> +84
                </a>
            </a-menu-item>
        </a-menu>
        <a-menu mode="horizontal" style="position: absolute; right: 0; top: 0; min-width: 245px;">
            <a-menu-item key="signup">
                <UserOutlined /> Đăng ký
            </a-menu-item>
            <a-menu-item key="login">
                <LockOutlined /> Đăng nhập
            </a-menu-item>
        </a-menu>
    </a-layout-header>
</template>
<script>
import {
    MessageOutlined, PhoneOutlined,
    UserOutlined, LockOutlined,
} from '@ant-design/icons-vue';

export default {
    components: {
        MessageOutlined, PhoneOutlined,
        UserOutlined, LockOutlined,
    },
}
</script>
