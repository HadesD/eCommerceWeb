<template>
    <a-layout-header style="background-color: blueviolet;">
        <router-link to="/">
            <div style="float: left;padding: 0 15px;">
                <img src="/favicon.ico" alt="RinPhone.vn logo" />
            </div>
        </router-link>
        <a-menu mode="horizontal" theme="dark" style="background-color: blueviolet;">
            <a-menu-item key="1" style="font-size: 20px;">
                <a href="https://fb.com/rinphonevn" target="_blank">
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
            <a-menu-item key="1" disabled><UserOutlined /> Đăng ký</a-menu-item>
            <a-menu-item key="2" disabled><LockFilled /> Đăng nhập</a-menu-item>
        </a-menu>
    </a-layout-header>
    <a-layout-content style="background-color:#FFF;">
        <a-row style="padding: 30px 0 20px;">
            <a-col :sm="{ span: 24 }" :lg="{ span: 16 }">
                <a-row :gutter="10">
                    <a-col :xs="{ span: 24 }" :md="{ span: 10 }" :lg="{ span: 8 }" :xl="{ span: 7 }">
                        <router-link :to="{name:'home'}">
                            <img src="/logo-blue.png" style="padding-bottom:5px;" />
                        </router-link>
                    </a-col>
                    <a-col :xs="{ span: 24 }" :md="{ span: 14 }" :lg="{ span: 16 }" :xl="{ span: 17 }">
                        <SearchProductForm />
                    </a-col>
                </a-row>
            </a-col>
            <a-col :sm="{ span: 24 }" :lg="{ span: 8 }" style="text-align: right;">
                <a-dropdown>
                    <a-tooltip placement="top" title="Tới trang Thanh Toán">
                        <router-link :to="{ name: 'cart' }">
                            <a-badge :count="cartItems.reduce((n, {num}) => (n + num), 0)">
                                <a-button type="primary" size="large">{{ money_format(cartItems.reduce((n, r) => (r.product.price * r.num + n), 0)) }} <ShoppingCartOutlined /></a-button>
                            </a-badge>
                        </router-link>
                    </a-tooltip>
                    <template #overlay>
                        <a-list item-layout="horizontal" :data-source="cartItems" style="min-width: 350px;" class="header-cart-dropdown">
                            <template #renderItem="{ item }">
                                <router-link :to="{ name: 'product', params: {
                                    category_slug: item.product.categories[0].slug,
                                    product_slug: vietnameseNormalize(item.product.name),
                                    product_id: item.product.id,
                                } }">
                                    <a-list-item key="item" :title="item.product.name">
                                        <a-list-item-meta>
                                            <template #description>
                                                <div>Giá: {{ money_format(item.product.price) }}</div>
                                                <div>Số lượng: {{ item.num }}</div>
                                            </template>
                                            <template #title>
                                                <a-typography-paragraph
                                                    :ellipsis="{
                                                        rows: 1,
                                                        expandable: false,
                                                    }"
                                                    :content="item.product.name"
                                                />
                                            </template>
                                        </a-list-item-meta>
                                        <template #extra>
                                            <img width="40" height="40" :alt="item.product.name" :src="item.product.images[0]?.url || '/favicon.ico'" />
                                        </template>
                                    </a-list-item>
                                </router-link>
                            </template>
                        </a-list>
                    </template>
                </a-dropdown>
            </a-col>
        </a-row>
    </a-layout-content>
    <a-layout-header style="background-color: #FFF;border-top: solid 1px #f2f2f2;box-shadow: 0px 5px 20px rgb(0 0 0 / 10%);">
        <a-menu mode="horizontal" :selectedKeys="[$route.params?.category_slug || '/']">
            <!-- <TreeMenu :nodeData="categories" /> -->
            <template v-for="category in categories">
                <a-menu-item><router-link :to="{name:'category', params:{category_slug:category.slug}}">{{ category.name }}</router-link></a-menu-item>
            </template>
        </a-menu>
    </a-layout-header>

    <a-layout-content v-if="$route.name === 'home'" style="margin-top: 25px;">
        <a-row :gutter="16">
            <a-col :xs="24" :sm="18" :lg="19">
                <img src="https://i.imgur.com/66IxckB.jpg" style="width: 100%;" />
            </a-col>
            <a-col :xs="24" :sm="6" :lg="5">
                <div class="fb-page" data-href="https://www.facebook.com/rinphonevn" :data-tabs="($grid.breakpoint !== 'xs') ? 'timeline' : ''">
                    <blockquote cite="https://www.facebook.com/rinphonevn" class="fb-xfbml-parse-ignore">
                        <a href="https://www.facebook.com/rinphonevn" target="_blank">RinPhoneVN Facebook</a>
                    </blockquote>
                </div>
            </a-col>
        </a-row>
    </a-layout-content>
</template>
<script>
import {
    computed,
    onMounted, ref,
    h, resolveComponent,
} from 'vue';

import {
    FacebookFilled, PhoneFilled,
    UserOutlined, LockFilled,
    ShoppingCartOutlined, MenuOutlined,
    CaretDownOutlined, HomeOutlined,
    LoadingOutlined,
} from '@ant-design/icons-vue';

import SearchProductForm from '../components/SearchProductForm';
import RequestRepository from '../utils/RequestRepository';
import { list_to_tree, money_format, vietnameseNormalize, } from '../helpers';
import { useStore } from 'vuex';
import { useGrid } from 'vue-screen';

const TreeMenu = {
    props: {
        nodeData: Array,
    },

    render() {
        const MenuItem = resolveComponent('a-menu-item');
        const SubMenu = resolveComponent('a-sub-menu');
        const RouterLink = resolveComponent('router-link');

        const renderItem = (node) => {
            const hasNode = node.children && (node.children.length > 0);
            const slugLink = h(RouterLink, {
                to: {
                    name: 'category',
                    params: {
                        category_slug: node.slug,
                    },
                },
            }, {
                default: () => node.name,
            });
            return h(hasNode ? SubMenu : MenuItem, hasNode ? {
                title: slugLink,
            } : {
                key: node.slug,
            }, {
                default: () => hasNode ? h(TreeMenu, {nodeData: node.children}) : slugLink,
            });
        };

        return this.nodeData.map(node => renderItem(node));
    },
};

export default {
    components: {
        FacebookFilled, PhoneFilled,
        UserOutlined, LockFilled,
        ShoppingCartOutlined, MenuOutlined,
        CaretDownOutlined, HomeOutlined,
        LoadingOutlined,

        SearchProductForm,
        TreeMenu,
    },

    setup() {
        const store = useStore();

        onMounted(() => {
            store.dispatch('fetchCartItems');
            store.dispatch('fetchCategories');
        });

        return {
            loadingCategories: computed(() => store.getters.getLoadingCategories),
            categories: computed(() => list_to_tree(store.getters.getCategories)),
            cartItems: computed(() => store.getters.getCartItems),

            money_format,
            vietnameseNormalize,
        };
    },
}
</script>
