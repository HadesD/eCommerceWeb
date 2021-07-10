<template>
    <a-layout-sider v-model="collapsed" id="slider_sidebar" :width="width" :collapsedWidth="collapsedWidth">
        <a-menu
            theme="dark"
            mode="inline"
            :openKeys="['/', '/statistics']"
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
                    <QuestionCircleOutlined />
                    <span>Hướng dẫn</span>
                </router-link>
            </a-menu-item>
            <a-sub-menu key="/">
                <template #title>
                    <ShopOutlined />
                    <span>Quản lý tiệm</span>
                </template>
                <a-menu-item key="/products/">
                    <router-link to="/products/">
                        <ShoppingCartOutlined />
                        <span>Sản phẩm</span>
                    </router-link>
                </a-menu-item>
                <a-menu-item key="/orders/">
                    <router-link to="/orders/">
                        <AccountBookOutlined />
                        <span>Hoá Đơn</span>
                    </router-link>
                </a-menu-item>
                <a-menu-item key="/stocks/">
                    <router-link to="/stocks/">
                        <BankOutlined />
                        <span>Kho hàng</span>
                    </router-link>
                </a-menu-item>
                <a-menu-item key="/users/">
                    <router-link to="/users/">
                        <UserOutlined />
                        <span>Khách hàng</span>
                    </router-link>
                </a-menu-item>
            </a-sub-menu>
            <a-sub-menu key="/statistics">
                <template #title>
                    <BarChartOutlined />
                    <span>Thống kê</span>
                </template>
                <a-menu-item key="/transactions/">
                    <router-link to="/transactions/">
                        <TransactionOutlined />
                        <span>Thu / Chi</span>
                    </router-link>
                </a-menu-item>
            </a-sub-menu>
        </a-menu>
    </a-layout-sider>
</template>
<script>
import {
    UserOutlined, BarChartOutlined, BankOutlined,
    AccountBookOutlined, QuestionCircleOutlined,
    ShopOutlined, ShoppingCartOutlined, TransactionOutlined,
} from '@ant-design/icons-vue';

export default {
    components: {
        UserOutlined, BarChartOutlined, BankOutlined,
        AccountBookOutlined, QuestionCircleOutlined,
        ShopOutlined, ShoppingCartOutlined, TransactionOutlined,
    },
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
