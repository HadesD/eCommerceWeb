<template>
    <a-typography-title :level="2">Kết quả tìm kiếm</a-typography-title>
    <ProductList />
</template>
<script>
import ProductList from '../components/ProductList.vue';

export default {
    components: {
        ProductList,
    },
}
</script>
