<template>
    <a-typography-title :level="1">{{ pageTitle }}</a-typography-title>
    <a-table
        :dataSource="cartItems"
        :columns="cartTableColumns"
        bordered
        :pagination="false"
    >
        <template #bodyCell="{ column, text, record }">
            <template v-if="column.key === 'img'">
                <img :src="record.product.images[0]?.url || '/favicon.ico'" :alt="record.product.name" style="display:block;margin:0 auto;max-width: 100px; max-height: 100px; padding: 2px; border: 1px solid #CCC;border-radius:3px;" />
            </template>

            <template v-if="column.key === 'name'">
                <router-link :to="{
                    name: 'product',
                    params: {
                        category_slug: record.product.categories[0]?.slug,
                        product_slug: vietnameseNormalize(record.product.name),
                        product_id: record.product.id,
                    },
                }">{{ record.product.name }}</router-link>
                <br />
                <a-tag color="purple">#{{ record.product.id }}</a-tag>
            </template>

            <template v-if="column.dataIndex === 'num'">
                <a-space>
                    <a-input-number :value="record.num" :min="1" @change="v => onItemNumChanged(record, v)" size="large" />
                    <a-popconfirm title="Bạn có chắc chắn muốn xoá sản phẩm khỏi giỏ hàng?" @confirm="() => onItemDelete(record)">
                        <a-button type="ghost" danger><template #icon><DeleteOutlined /></template></a-button>
                    </a-popconfirm>
                </a-space>
            </template>
        </template>
        <template #footer>
            <a-typography-title :level="2" style="text-align: right;margin-bottom:0;">
                <span>Tổng: </span>
                <span style="color: red;">{{ money_format(totalPrice()) }}</span>
            </a-typography-title>
        </template>
    </a-table>
    <div style="text-align:right; margin-top: 15px;">
        <a-button type="primary" size="large" @click="() => onCheckout()" :disabled="!cartItems.length" :loading="checkingOut">
            <template #icon><DollarOutlined /></template>
            <span>Thanh toán</span>
        </a-button>
    </div>
</template>
<script>
import { computed, ref } from '@vue/reactivity';
import { useStore } from 'vuex';
import { onMounted } from '@vue/runtime-core';

import {
    DeleteOutlined,
    DollarOutlined,
} from '@ant-design/icons-vue';

import RequestRepository from '~/utils/RequestRepository';
import RequestCsrfToken from '~/utils/RequestCsrfToken';
import { money_format, vietnameseNormalize, showErrorRequestApi, } from '../helpers';

const cartTableColumns = [
    {
        key: 'img',
    },
    {
        key: 'name',
        title: 'Sản phẩm',
    },
    {
        title: 'Đơn giá',
        dataIndex: ['product', 'price'],
        customRender: ({text}) => money_format(text),
    },
    {
        title: 'Số lượng',
        dataIndex: 'num',
    },
    {
        key: 'lastPrice',
        title: 'Thành tiền',
        customRender: ({record}) => money_format(record.product.price * record.num),
    },
];

export default {
    components: {
        DeleteOutlined,
        DollarOutlined,
    },

    setup() {
        const store = useStore();

        const pageTitle = 'Giỏ hàng';
        const checkingOut = ref(false);
        const cartItems = computed(() => store.getters.getCartItems);

        const totalPrice = () => {
            return cartItems.value.reduce((n, r) => n + (r.num * r.product.price), 0);
        };

        const onItemDelete = (item) => {
            store.dispatch('removeCartItem', item);
        };

        const onItemNumChanged = (item, val) => {
            store.dispatch('updateCartItem', {
                ...item,
                num: val,
            });
        };

        const onCheckout = () => {
            checkingOut.value = true;

            RequestCsrfToken()
                .then(() => {
                    checkingOut.value = true;
                    RequestRepository.post('/orders', cartItems.value.map(v => ({
                        num: v.num,
                        product_id: v.product.id,
                        // TODO: Add variants properties
                    })))
                        .then(data => {
                            console.log(data);

                            store.dispatch('clearCartItems');
                        })
                        .catch(showErrorRequestApi)
                        .finally(() => (checkingOut.value = false));
                })
                .catch(showErrorRequestApi)
                .finally(() => (checkingOut.value = false));
        };

        onMounted(() => {
            document.title = pageTitle;
        });

        return {
            pageTitle,
            cartItems,
            checkingOut,

            cartTableColumns,

            totalPrice,
            onItemNumChanged,
            onItemDelete,
            onCheckout,

            money_format,
            vietnameseNormalize,
        };
    },
}
</script>
