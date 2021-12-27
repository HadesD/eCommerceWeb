<template>
    <a-typography-title :level="2">Kết quả tìm kiếm</a-typography-title>
    <ProductList
        :keyword="$route.query.keyword"
    />
</template>
<script>
import { onMounted } from '@vue/runtime-core';
import ProductList from '../components/ProductList.vue';
import { useRoute } from 'vue-router';

export default {
    components: {
        ProductList,
    },

    setup() {
        const route = useRoute();

        onMounted(() => {
            document.title = `Tìm kiếm: ${route.query.keyword}`;
        });
    },
}
</script>
