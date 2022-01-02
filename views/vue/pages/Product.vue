<template>
        <metainfo>
            <template v-slot:title="{ content }">{{ content ? `${content} | SITE_NAME` : `SITE_NAME` }}</template>
        </metainfo>
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
            <a-space direction="vertical">
                <a-typography-text v-for="category in product.categories">
                    <router-link
                        :to="{
                            name: 'category',
                            params: {
                                category_slug: category.slug,
                            }
                        }"
                    >{{ category.name }}</router-link>
                </a-typography-text>
            </a-space>
            <a-typography-title :level="2">{{ product.name }}</a-typography-title>
        </a-col>
    </a-row>
</template>
<script>
import { onMounted, reactive, ref, watch, } from 'vue';
import { useRoute, useRouter } from 'vue-router'
import { useMeta } from 'vue-meta';

import RequestRepository from '~/utils/RequestRepository';
import { vietnameseNormalize } from '~/helpers';

const baseUrl = 'https://raw.githubusercontent.com/vueComponent/ant-design-vue/next/components/vc-slick/assets/img/react-slick/';

export default {
    setup() {
        const route = useRoute();
        const router = useRouter();

        const product = ref({});

        const loadProduct = () => {
            RequestRepository.get('/products/' + route.params.product_id)
                .then(res => {
                    const productData = res.data.data;

                    document.title = productData.name;

                    const productSlug = vietnameseNormalize(productData.name);
                    if (route.params.product_slug !== productSlug) {
                        router.replace({
                            params: {
                                ...route.params,
                                product_slug: productSlug,
                            },
                        });
                    }

                    product.value = productData;
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
            (value) => {
                if (value) {
                    loadProduct();
                }
            }
        );

        return {
            product,

            baseUrl,
            getImgUrl,
        };
    },
}
</script>
