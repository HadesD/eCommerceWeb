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
        title: '',
        dataIndex: 'product',
    },
];

export default {
    setup() {
        const store = useStore();

        return {
            cartItems: computed(() => store.getters.getCartItems),

            cartTableColumns,
        };
    },
}
</script>
