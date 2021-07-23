<template>
    <a-row :gutter="16">
        <a-col :md="24" :lg="4">
            <a-typography-paragraph>
                <a-typography-title :level="4">Chuyên mục</a-typography-title>
                <a-spin :spinning="loadingCategories">
                    <ul v-if="(categories.length > 0)">
                        <li v-for="cat in categories" :key="cat.id">
                            <router-link :to="{ name: 'category', params: { category_slug: cat.slug } }">
                                <a-typography-text type="secondary">{{ cat.name }}</a-typography-text>
                            </router-link>
                        </li>
                    </ul>
                    <!-- <a-empty v-else /> -->
                </a-spin>

                <a-typography-title :level="4">Bộ lọc</a-typography-title>
                <h5>Giá tiền</h5>
                <a-slider range :step="5" :max="50000" v-model:value="priceRange" @afterChange="onPriceRangeAfterChange" />
                <a-row :gutter="8">
                    <a-col :span="12">
                        <a-input-number v-model:value="priceRange[0]" :formatter="value => `${number_format(value*1000)} ₫`" style="width:100%" />
                    </a-col>
                    <a-col :span="12">
                        <a-input-number v-model:value="priceRange[1]" :formatter="value => `${number_format(value*1000)} ₫`" style="width:100%" />
                    </a-col>
                </a-row>
            </a-typography-paragraph>

        </a-col>
        <a-col :md="24" :lg="20">
            <ProductList
                :categorySlug="$route.params?.category_slug"
                :priceRange="priceRangeAfterChange.map(value => (value * 1000))"
            />
        </a-col>
    </a-row>
</template>
<script>
import {
    onMounted, ref, watch,
} from 'vue';
import { useRoute, } from 'vue-router';

import {
    number_format,
} from '../helpers';

import RequestRepository from '../utils/RequestRepository';
import ProductList from '../components/ProductList';
import { debounce } from 'lodash';

export default {
    components: {
        ProductList,
    },

    setup() {
        const route = useRoute();

        const categories = ref([]);
        const loadingCategories = ref(false);
        const priceRange = ref([0, 50000]);
        const priceRangeAfterChange = ref([]);

        const loadCategories = (slug) => {
            loadingCategories.value = true;

            RequestRepository.get('/categories', {
                params: {
                    parent_slug: slug,
                },
            })
                .then(res => {
                    categories.value = res.data.data;
                })
                .finally(() => { loadingCategories.value = false });
        }

        const onPriceRangeAfterChange = debounce((value) => {
            priceRangeAfterChange.value = value;
        }, 500);

        onMounted(() => {
            loadCategories(route.params.category_slug);
        });

        watch(
            () => route.params.category_slug,
            newSlug => loadCategories(newSlug)
        );

        return {
            categories,
            priceRange,
            priceRangeAfterChange,

            loadingCategories,

            onPriceRangeAfterChange,
            number_format,
        };
    },
}
</script>
