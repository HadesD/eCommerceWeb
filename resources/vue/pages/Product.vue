<template>
    {{ product.name }}
</template>
<script>
import { onMounted, reactive, ref, watch, } from 'vue';
import { useRoute, useRouter } from 'vue-router'

import RequestRepository from '../utils/RequestRepository';

export default {
    setup() {
        const route = useRoute();

        const product = reactive({});

        const loadProduct = () => {
            RequestRepository.get('/products/' + route.params.product_slug)
                .then(res => {
                    product.value = res.data.data;
                });
        };

        onMounted(() => {
            loadProduct();
        });

        watch(
            () => route.params.product_slug,
            () => loadProduct()
        );

        return {
            product,
        };
    },
}
</script>
