<template>
    <div ref="productListRow">
        <div v-if="loadingProductList">
            <a-skeleton />
            <a-skeleton />
            <a-skeleton />
        </div>
        <div v-else>
            <a-row>
                <a-col :span="12">
                    <a-typography-title :level="4">Có <a-tag color="purple">{{ number_format(products.total) }}</a-tag> sản phẩm</a-typography-title>
                </a-col>
                <a-col :span="12" style="text-align: right;">
                    <span>Sắp xếp: </span>
                    <a-select
                        size="small"
                        style="min-width: 120px;"
                        v-model:value="sortBy"
                        @change="onSortByChange"
                    >
                        <a-select-option value="+price">Rẻ nhất</a-select-option>
                        <a-select-option value="-price">Đắt nhất</a-select-option>
                        <a-select-option value="-created_at">Mới đăng bán</a-select-option>
                        <a-select-option value="+name">Tên sản phẩm</a-select-option>
                    </a-select>
                </a-col>
            </a-row>

            <a-row :gutter="16" v-if="(products.data?.length > 0)">
                <a-col :xs="24" :sm="24" :md="8" :lg="6" :xl="4" v-for="product in products?.data" :key="product.id" style="margin-bottom: 16px;">
                    <router-link
                        :to="{
                            name: 'product',
                            params: {
                                category_slug: $route.params.category_slug || product.categories[0].slug,
                                product_slug: vietnameseNormalize(product.name),
                                product_id: product.id,
                            }
                        }"
                    >
                        <a-card hoverable>
                            <template #cover>
                                <img :alt="product.name" :src="product.images.length ? product.images[0].url : '/favicon.ico'" />
                            </template>
                            <template class="ant-card-actions" #actions>
                                <!-- <setting-outlined key="setting" />
                                <edit-outlined key="edit" />
                                <ellipsis-outlined key="ellipsis" /> -->
                            </template>
                            <a-card-meta :title="money_format(product.price)">
                                <template #description>
                                    <a-typography-paragraph
                                        :ellipsis="{ rows: 2, expandable: false, }"
                                        :title="product.name"
                                        style="height: 3em;"
                                        :content="product.name"
                                    />
                                </template>
                            </a-card-meta>
                        </a-card>
                    </router-link>
                </a-col>
            </a-row>
            <a-empty v-else />

            <a-pagination
                v-model:current="products.current_page"
                :total="products.total"
                :pageSize="products.per_page"
                @change="(page) => loadProductList({ page })"
            />
        </div>
    </div>
</template>
<script>
import { onMounted, ref, watch } from 'vue';
import { message } from 'ant-design-vue';

import RequestRepository from '~/utils/RequestRepository';

import {
    money_format,
    number_format,
    vietnameseNormalize,
} from '~/helpers';
import { useRoute, useRouter } from 'vue-router';
import { showErrorRequestApi } from '../helpers';
import { debounce } from 'lodash';

export default {
    props: {
        categorySlug: String,
        keyword: String,
        priceRange: Array,
    },

    setup(props) {
        const productListRow = ref();
        const products = ref({});
        const loadingProductList = ref(false);
        const sortBy = ref('-created_at');
        const router = useRouter();
        const route = useRoute();

        const loadProductList = ({ page, }) => {
            if (page) {
                router.push({
                    query: {
                        ...route.query,
                        page,
                    },
                });
            }

            loadingProductList.value = true;

            RequestRepository.get('/products', {
                params: {
                    page,
                    keyword: props.keyword,
                    category_slug: props.categorySlug,
                    sort_by: sortBy.value,
                    price_range: (props.priceRange && props.priceRange.length) ? (props.priceRange[0] + ',' + props.priceRange[1]) : undefined,
                },
            })
                .then(res => {
                    products.value = res.data;
                })
                .catch(showErrorRequestApi)
                .finally(() => {
                    loadingProductList.value = false;
                });
        };

        const onSortByChange = (value) => {
            sortBy.value = value;
        };

        onMounted(() => {
            loadProductList({ page: route.query.page });
        });

        watch(() => props.categorySlug, () => loadProductList({}));

        watch(() => props.keyword, () => loadProductList({}));

        watch(() => props.priceRange, () => loadProductList({}));

        watch(() => sortBy.value, () => loadProductList({}));

        watch(() => products.value, debounce(() => productListRow.value.scrollIntoView({behavior: 'smooth'}), 100));

        return {
            productListRow,

            products,
            loadingProductList,

            sortBy,

            loadProductList,
            onSortByChange,

            money_format,
            number_format,
            vietnameseNormalize,
        };
    },
}
</script>
