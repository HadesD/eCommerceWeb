<template>
    <a-typography-title :level="1">{{ category.name }}</a-typography-title>
    <a-row :gutter="16">
        <a-col :md="24" :lg="4">
            <a-typography-paragraph>
                <a-typography-title :level="4">Chuyên mục con</a-typography-title>
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
import { debounce } from 'lodash';
import { message } from 'ant-design-vue';

import { number_format, showErrorRequestApi, } from '~/helpers';

import RequestRepository from '~/utils/RequestRepository';

import ProductList from '~/components/ProductList.vue';

export default {
    components: {
        ProductList,
    },

    setup() {
        const route = useRoute();

        const categories = ref([]);
        const category = ref({
            name: null,
            slug: null,
        });
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
                    const resData = res.data;

                    category.value.name = resData.name;
                    category.value.slug = slug;

                    categories.value = resData.data;
                })
                .catch(showErrorRequestApi)
                .finally(() => { loadingCategories.value = false });
        }

        const onPriceRangeAfterChange = debounce((value) => {
            priceRangeAfterChange.value = value.map(v => (v * 1000));
        }, 500);

        const onChangePriceRangeF = (value) => {
            priceRange.value = [value, priceRange.value[1]];
        };

        const onChangePriceRangeT = (value) => {
            priceRange.value = [priceRange.value[0], value];
        };

        onMounted(() => {
            loadCategories(route.params.category_slug);
        });

        watch(
            () => route.params.category_slug,
            value => {
                if (value) {
                    loadCategories(value);
                }
            }
        );

        watch(
            () => category.value.name,
            value => {
                document.title = value;
            }
        );

        return {
            category,
            categories,
            priceRange,
            priceRangeAfterChange,

            loadingCategories,

            onPriceRangeAfterChange,
            onChangePriceRangeF,
            onChangePriceRangeT,
            number_format,
        };
    },
}
</script>
