<template>
    <a-row>
        <a-col :xs="24" :sm="24" :md="24" :lg="12">
            <a-carousel arrows dots-class="slick-dots slick-thumb" class="product-carousel">
                <template #customPaging="props">
                    <img :src="getImgUrl(props.i)" />
                </template>
                <div v-for="item in 4" :key="item">
                    <img :src="baseUrl + 'abstract0' + item + '.jpg'" :alt="product.name" />
                </div>
            </a-carousel>
        </a-col>
        <a-col :xs="24" :sm="24" :md="24" :lg="12">
            {{ product.name }}
        </a-col>
    </a-row>
</template>
<script>
import { onMounted, reactive, ref, watch, } from 'vue';
import { useRoute, useRouter } from 'vue-router'

import RequestRepository from '../utils/RequestRepository';

const baseUrl = 'https://raw.githubusercontent.com/vueComponent/ant-design-vue/next/components/vc-slick/assets/img/react-slick/';

export default {
    setup() {
        const route = useRoute();

        const product = ref({});

        const loadProduct = () => {
            RequestRepository.get('/products/' + route.params.product_slug)
                .then(res => {
                    product.value = res.data.data;
                });
        };

        const getImgUrl = (i) => {
            return `${baseUrl}abstract0${i + 1}.jpg`;
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

            baseUrl,
            getImgUrl,
        };
    },
}
</script>
