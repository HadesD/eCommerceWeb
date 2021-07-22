<template>
    <div v-for="cat in categories">{{ cat.name }}</div>
</template>
<script>
import {
    onMounted, ref, watch
} from 'vue';
import RequestRepository from '../utils/RequestRepository';
import { useRoute, } from 'vue-router';

export default {
    setup() {
        const route = useRoute();

        const categories = ref([]);

        const loadCategories = (slug) => {
            RequestRepository.get('/categories', {
                params: {
                    parent_slug: slug,
                },
            })
                .then(res => [
                    categories.value = res.data.data,
                ]);
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
        };
    },
}
</script>
