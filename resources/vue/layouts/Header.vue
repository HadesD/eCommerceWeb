<template>
    <a-layout-header style="background-color: blueviolet;">
        <router-link to="/">
            <div style="float: left;padding: 0 15px;">
                <img src="/favicon.ico" />
            </div>
        </router-link>
        <a-menu mode="horizontal" theme="dark" style="background-color: blueviolet;">
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
        <a-menu mode="horizontal" theme="dark" style="background-color: blueviolet;position: absolute; right: 0; top: 0; min-width: 245px;">
            <a-menu-item key="1"><UserOutlined /> Đăng ký</a-menu-item>
            <a-menu-item key="2"><LockFilled /> Đăng nhập</a-menu-item>
        </a-menu>
    </a-layout-header>
    <a-layout-content style="background-color:#FFF;">
        <a-row style="padding: 30px 0 20px;">
            <a-col :sm="{ span: 24 }" :lg="{ span: 16 }">
                <a-row :gutter="10">
                    <a-col :xs="{ span: 12 }" :md="{ span: 10 }" :lg="{ span: 6 }" :xl="{ span: 5 }">
                        <span style="font-size: 30px;"><img src="/favicon.ico" />inPhone</span>
                    </a-col>
                    <a-col :xs="{ span: 12 }" :md="{ span: 14 }" :lg="{ span: 18 }" :xl="{ span: 19 }">
                        <SearchProductForm />
                    </a-col>
                </a-row>
            </a-col>
            <a-col :sm="{ span: 24 }" :lg="{ span: 8 }" style="text-align: right;">
                <a-dropdown>
                    <a-badge :count="5">
                        <a-button type="primary" size="large">0 ₫ <ShoppingCartOutlined /></a-button>
                    </a-badge>
                    <template #overlay>
                        <a-menu>
                            <a-menu-item key="1">
                                <UserOutlined />
                                1st menu item
                            </a-menu-item>
                            <a-menu-item key="2">
                                <UserOutlined />
                                2nd menu item
                            </a-menu-item>
                            <a-menu-item key="3">
                                <UserOutlined />
                                3rd item
                            </a-menu-item>
                        </a-menu>
                    </template>
                </a-dropdown>
            </a-col>
        </a-row>
    </a-layout-content>
    <a-layout-header style="background-color: #FFF;border-top: solid 1px #f2f2f2;box-shadow: 0px 5px 20px rgb(0 0 0 / 10%);">
        <a-menu mode="horizontal">
            <a-sub-menu>
                <template #icon><MenuOutlined /></template>
                <template #title>CHUYÊN MỤC <CaretDownOutlined /></template>
                <a-menu-item-group title="Item 1">
                    <a-menu-item key="setting:1">Option 1</a-menu-item>
                    <a-menu-item key="setting:2">Option 2</a-menu-item>
                </a-menu-item-group>
                <a-menu-item-group title="Item 2">
                    <a-menu-item key="setting:3">Option 3</a-menu-item>
                    <a-menu-item key="setting:4">Option 4</a-menu-item>
                </a-menu-item-group>
            </a-sub-menu>
            <a-menu-item key="2">
                <router-link to="/">Trang Chủ</router-link>
            </a-menu-item>
        </a-menu>
    </a-layout-header>
</template>
<script>
import {
    FacebookFilled, PhoneFilled,
    UserOutlined, LockFilled,
    ShoppingCartOutlined, MenuOutlined,
    CaretDownOutlined,
} from '@ant-design/icons-vue';

import SearchProductForm from '../components/SearchProductForm';

export default {
    components: {
        FacebookFilled, PhoneFilled,
        UserOutlined, LockFilled,
        ShoppingCartOutlined, MenuOutlined,
        CaretDownOutlined,
        SearchProductForm,
    },
}
</script>
