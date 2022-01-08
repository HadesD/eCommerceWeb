<template>
    <a-table
        :dataSource="cartItems"
        :columns="cartTableColumns"
    >
    </a-table>
</template>
<script>
import { computed } from '@vue/reactivity';
import { useStore } from 'vuex';

const cartTableColumns = [
    {
        title: 'Mã sản phẩm',
        dataIndex: 'product[id]',
    },
    {
        title: 'Đơn giá',
        title: 'product.price',
    },
    {
        title: 'Số lượng',
        dataIndex: 'num',
    },
];

export default {
    setup() {
        const store = useStore();

        console.log(store.getters.getCartItems);

        return {
            cartItems: computed(() => store.getters.getCartItems),

            cartTableColumns,
        };
    },
}
</script>
