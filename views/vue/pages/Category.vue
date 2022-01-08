<template>
    <a-breadcrumb>
        <a-breadcrumb-item>
            <HomeOutlined />
        </a-breadcrumb-item>
        <a-breadcrumb-item v-for="cat in breadcrums" :key="cat.id">
            <router-link v-if="cat.id !== category.id" :to="{ name: 'category', params: { category_slug: cat.slug } }">{{ cat.name }}</router-link>
            <a-typography-title v-else style="display: inline-block;" :level="1">{{ category.name }}</a-typography-title>
        </a-breadcrumb-item>
    </a-breadcrumb>
    <a-row :gutter="16">
        <a-col :md="24" :lg="4">
            <a-typography-paragraph>
                <a-typography-title :level="4">Chuyên mục con</a-typography-title>
                <ul v-if="(categories.length > 0)">
                    <li v-for="cat in categories" :key="cat.id">
                        <router-link :to="{ name: 'category', params: { category_slug: cat.slug } }">
                            <a-typography-text type="secondary">{{ cat.name }}</a-typography-text>
                        </router-link>
                    </li>
                </ul>

                <a-typography-title :level="4">Bộ lọc</a-typography-title>
                <strong>Giá tiền</strong>
                <a-slider range :step="5" :max="50000" v-model:value="priceRange" @afterChange="onPriceRangeAfterChange" />
                <a-row :gutter="8">
                    <a-col :span="12">
                        <a-input-number
                            :step="5"
                            :min="0"
                            :max="45000"
                            v-model:value="priceRange[0]"
                            :formatter="value => `${number_format(value*1000)} ₫`"
                            :parser="value => value.replace(' ₫', '').replace(/\./g,'')"
                            style="width:100%"
                            @change="onChangePriceRangeF"
                        />
                    </a-col>
                    <a-col :span="12">
                        <a-input-number
                            :step="5"
                            :min="5"
                            :max="50000"
                            v-model:value="priceRange[1]"
                            :formatter="value => `${number_format(value*1000)} ₫`"
                            :parser="value => value.replace(' ₫', '').replace(/\./g,'')"
                            style="width:100%"
                            @change="onChangePriceRangeT"
                        />
                    </a-col>
                </a-row>
            </a-typography-paragraph>

        </a-col>
        <a-col :md="24" :lg="20">
            <ProductList
                :categorySlug="$route.params?.category_slug"
                :priceRange="priceRangeAfterChange"
            />
        </a-col>
    </a-row>
</template>
<script>
import { onMounted, ref, watch, } from 'vue';
import { useRoute, } from 'vue-router';
import { useStore } from 'vuex';

import { debounce } from 'lodash';

import { message } from 'ant-design-vue';
import { HomeOutlined, } from '@ant-design/icons-vue';

import { number_format, showErrorRequestApi, } from '~/helpers';

import RequestRepository from '~/utils/RequestRepository';

import ProductList from '~/components/ProductList.vue';

export default {
    components: {
        ProductList,
        HomeOutlined,
    },

    setup() {
        const route = useRoute();
        const store = useStore();

        const categories = ref([]);
        const breadcrums = ref([]);
        const category = ref({});
        const priceRange = ref([0, 50000]);
        const priceRangeAfterChange = ref([]);

        const onPriceRangeAfterChange = debounce((value) => {
            priceRangeAfterChange.value = value.map(v => (v * 1000));
        }, 500);

        const onChangePriceRangeF = (value) => {
            priceRange.value = [value, priceRange.value[1]];
        };

        const onChangePriceRangeT = (value) => {
            priceRange.value = [priceRange.value[0], value];
        };

        const loadSubCats = (parentSlug) => {
            categories.value = [];
            breadcrums.value = [];

            const cats = store.getters.getCategories;
            const cat = category.value = cats.find(elm => elm.slug === parentSlug) || {};
            if (cat.id) {
                document.title = cat.name;
                for (const i in cats) {
                    const curCat = cats[i];
                    if (curCat.parent_id === cat.id) {
                        categories.value.push(curCat);
                        continue;
                    }

                    // Because we 're sorted in back-end, so we can break
                    if (categories.value.length) {
                        break;
                    }
                }

                const catsToBreadcrums = (parent_id) => {
                    const pCat = cats.find(elm => elm.id === parent_id);
                    if (pCat) {
                        if (pCat.parent_id) {
                            catsToBreadcrums(pCat.parent_id);
                        }

                        breadcrums.value.push(pCat);
                    }
                };

                catsToBreadcrums(cat.parent_id);

                // Last me
                breadcrums.value.push(cat);
            }
        };

        onMounted(() => {
            loadSubCats(route.params.category_slug);
        });

        watch(() => route.params.category_slug, loadSubCats);

        watch(() => store.getters.getCategories, () => loadSubCats(route.params.category_slug));

        return {
            category,
            breadcrums,
            categories,
            priceRange,
            priceRangeAfterChange,

            onPriceRangeAfterChange,
            onChangePriceRangeF,
            onChangePriceRangeT,
            number_format,
        };
    },
}
</script>
