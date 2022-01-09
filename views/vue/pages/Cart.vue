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
    <a-form style="text-align:right; margin-top: 15px;">
        <a-form-item label="Hình thức thanh toán">
            <a-select v-model:value="formData.payment_method">
                <a-select-option v-for="methodCode in Object.keys(configPaymentMethod)" :key="methodCode" :value="parseInt(methodCode)">{{ configPaymentMethod[methodCode].name }}</a-select-option>
            </a-select>
        </a-form-item>
        <a-form-item>
            <a-textarea v-model:value="formData.note" show-count :maxlength="300" placeholder="Ghi chú, yêu cầu cá nhân của quý khách đối với đơn hàng nếu có,..." />
        </a-form-item>
        <a-form-item>
            <a-button type="primary" size="large" @click="() => onCheckout()" :disabled="!cartItems.length" :loading="checkingOut">
                <template #icon><DollarOutlined /></template>
                <span>Thanh toán</span>
            </a-button>
        </a-form-item>
    </a-form>
</template>
<script>
import { computed, reactive, ref } from '@vue/reactivity';
import { useStore } from 'vuex';
import { useRouter } from 'vue-router';
import { onMounted } from '@vue/runtime-core';

import {
    DeleteOutlined,
    DollarOutlined,
} from '@ant-design/icons-vue';

import RequestRepository from '~/utils/RequestRepository';
import RequestCsrfToken from '~/utils/RequestCsrfToken';
import { money_format, vietnameseNormalize, showErrorRequestApi, } from '../helpers';
import PaymentMethod, { Config as configPaymentMethod } from '~/configs/PaymentMethod';

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
        const router = useRouter();

        const pageTitle = 'Giỏ hàng';
        const checkingOut = ref(false);
        const cartItems = computed(() => store.getters.getCartItems);
        const formData = reactive({
            note: null,
            payment_method: PaymentMethod.PM_ONCE,
        });

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
                    RequestRepository.post('/orders', {
                        ...formData,
                        items: cartItems.value.map(v => ({
                            num: v.num,
                            product_id: v.product.id,
                            // TODO: Add variants properties
                        })),
                    })
                        .then(data => {
                            store.dispatch('clearCartItems');

                            const order = data.data.data;

                            router.push({
                                name: 'order',
                                params: {
                                    order_id: order.id,
                                },
                            });
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
            formData,

            cartTableColumns,

            totalPrice,
            onItemNumChanged,
            onItemDelete,
            onCheckout,

            PaymentMethod,
            configPaymentMethod,

            money_format,
            vietnameseNormalize,
        };
    },
}
</script>
