<template>
    <a-row :gutter="16">
        <a-col :span="4">
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
            </a-typography-paragraph>
        </a-col>
        <a-col :span="20">
            <ProductList
                :categorySlug="$route.params?.category_slug"
            />
        </a-col>
    </a-row>
</template>
<script>
import {
    onMounted, ref, watch
} from 'vue';
import { useRoute, } from 'vue-router';

import { useGrid, useScreen } from 'vue-screen';

import RequestRepository from '../utils/RequestRepository';
import ProductList from '../components/ProductList';

export default {
    components: {
        ProductList,
    },

    setup() {
        const route = useRoute();

        const categories = ref([]);
        const loadingCategories = ref(false);

        const screen = useScreen();
        const grid = useGrid('bootstrap');

        const loadCategories = (slug) => {
            console.log(screen, grid);
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

        onMounted(() => {
            loadCategories(route.params.category_slug);
        });

        watch(
            () => route.params.category_slug,
            newSlug => loadCategories(newSlug)
        );

        return {
            categories,
            loadingCategories,
        };
    },
}
</script>
