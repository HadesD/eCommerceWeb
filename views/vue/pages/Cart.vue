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
    <a-form
        style="margin-top: 15px;"
        :labelCol="{span:24}"
        :model="formData"
        @finish="onCheckout"
        :rules="formRules"
    >
        <a-card title="Thông tin thanh toán" style="margin-bottom: 15px;" :headStyle="{backgroundColor:'blueviolet',color:'#fff'}">
            <a-row :gutter="24">
                <a-col :span="12">
                    <a-form-item label="Họ và Tên" :name="['payment', 'fullname']">
                        <a-input :maxlength="50" v-model:value="formData.payment.fullname" placeholder="VD: Lê Văn A" />
                    </a-form-item>
                </a-col>
                <a-col :span="12">
                    <a-form-item label="Địa chỉ / Tên Facebook" :name="['payment', 'facebook']">
                        <a-input :maxlength="50" v-model:value="formData.payment.facebook" placeholder="VD: rinphonevn" />
                    </a-form-item>
                </a-col>
                <a-col :span="12">
                    <a-form-item label="Số điện thoại" :name="['payment', 'phone']">
                        <a-input :maxlength="20" v-model:value="formData.payment.phone" placeholder="VD: 0774475777" />
                    </a-form-item>
                </a-col>
                <a-col :span="12">
                    <a-form-item label="Địa chỉ E-Mail" :name="['payment', 'email']">
                        <a-input :maxlength="20" v-model:value="formData.payment.email" placeholder="VD: diachi@email.com" email />
                    </a-form-item>
                </a-col>
            </a-row>
            <a-form-item label="Hình thức thanh toán" :name="['payment', 'method']" help="Thanh toán trực tuyến sắp được triển khai">
                <a-select v-model:value="formData.payment.method">
                    <a-select-option v-for="methodCode in Object.keys(configPaymentMethod)" :key="methodCode" :value="parseInt(methodCode)">{{ configPaymentMethod[methodCode].name }}</a-select-option>
                </a-select>
            </a-form-item>
        </a-card>
        <a-form-item name="note">
            <a-textarea v-model:value="formData.note" show-count :maxlength="300" placeholder="Ghi chú, yêu cầu cá nhân của quý khách đối với đơn hàng nếu có,..." />
        </a-form-item>
        <a-form-item style="text-align:right;">
            <a-button type="primary" size="large" htmlType="submit" :disabled="!cartItems.length" :loading="checkingOut">
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

            payment: {
                phone: null,
                email: null,
                fullname: null,
                facebook: null,
                method: PaymentMethod.PM_ONCE,
            },
        });
        const formRules = reactive({
            payment: {
                phone: {
                    required: true,
                },
                method: {
                    required: true,
                },
            },
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

        const onCheckout = (values) => {
            checkingOut.value = true;

            RequestCsrfToken()
                .then(() => {
                    checkingOut.value = true;
                    RequestRepository.post('/orders', {
                        ...values,
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
            formRules,

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
