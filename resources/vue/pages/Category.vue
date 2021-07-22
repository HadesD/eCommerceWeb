<template>
    <a-row :gutter="16">
        <a-col :span="4">
            <a-typography-paragraph v-if="loadingCategories || (categories.length > 0)">
                <a-typography-title :level="5">Chuyên mục</a-typography-title>

                <ul v-if="(categories.length > 0)">
                    <li v-for="cat in categories" :key="cat.id">

                        <router-link :to="{ name: 'category', params: { slug: cat.slug } }">
                            <a-typography-text type="secondary">{{ cat.name }}</a-typography-text>
                        </router-link>
                    </li>
                </ul>
                <a-spin v-if="loadingCategories" />
            </a-typography-paragraph>
        </a-col>
        <a-col :span="20">
            <Search />
        </a-col>
    </a-row>
</template>
<script>
import {
    onMounted, ref, watch
} from 'vue';
import { useRoute, } from 'vue-router';

import RequestRepository from '../utils/RequestRepository';
import Search from './Search.vue';

export default {
    components: {
        Search,
    },

    setup() {
        const route = useRoute();

        const categories = ref([]);
        const loadingCategories = ref(false);

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

        onMounted(() => {
            loadCategories(route.params.slug);
        });

        watch(
            () => route.params.slug,
            newSlug => loadCategories(newSlug)
        );

        return {
            categories,
            loadingCategories,
        };
    },
}
</script>
