<template>
    <a-layout-sider v-model="collapsed" id="slider_sidebar" :width="width" :collapsedWidth="collapsedWidth">
        <a-menu
            theme="dark"
            mode="inline"
            :defaultOpenKeys="['/', '/statistics']"
            :selectedKeys="[$route.path]"
            :style="{ height: '100%', borderRight: 0 }"
        >
            <a-menu-item key="-" style="padding:0;">
                <router-link to="/">
                    <div class="logo">
                        <img src="/favicon.ico" /><span v-if="!collapsed" style="color:#FFF;font-size:25px;font-weight:bold;vertical-align: bottom;">inPhone.vn</span>
                    </div>
                </router-link>
            </a-menu-item>
            <a-menu-item key="/help/">
                <router-link to="/help/">
                    <a-icon type="question-circle" />
                    <span>Hướng dẫn</span>
                </router-link>
            </a-menu-item>
            <a-sub-menu key="/">
                <span slot="title">
                    <a-icon type="shop" />
                    <span>Quản lý tiệm</span>
                </span>
                <a-menu-item key="/products/">
                    <router-link to="/products/">
                        <a-icon type="shopping-cart" />
                        <span>Sản phẩm</span>
                    </router-link>
                </a-menu-item>
                <a-menu-item key="/orders/">
                    <router-link to="/orders/">
                        <a-icon type="account-book" />
                        <span>Hoá Đơn</span>
                    </router-link>
                </a-menu-item>
                <a-menu-item key="/stocks/">
                    <router-link to="/stocks/">
                        <a-icon type="bank" />
                        <span>Kho hàng</span>
                    </router-link>
                </a-menu-item>
                <a-menu-item key="/users/">
                    <router-link to="/users/">
                        <a-icon type="user" />
                        <span>Khách hàng</span>
                    </router-link>
                </a-menu-item>
            </a-sub-menu>
            <a-sub-menu key="/statistics">
                <span slot="title">
                    <a-icon type="pie-chart" />
                    <span>Thống kê</span>
                </span>
                <a-menu-item key="/transactions/">
                    <router-link to="/transactions/">
                        <a-icon type="transaction" />
                        <span>Thu / Chi</span>
                    </router-link>
                </a-menu-item>
            </a-sub-menu>
        </a-menu>
    </a-layout-sider>
</template>
<script>
export default {
    props: {
        width: {
            type: Number,
        },
        collapsedWidth: {
            type: Number,
        },
        collapsed: {
            type: Boolean,
            default: false,
        }
    },
    data() {
        return {
        }
    },
}
</script>
