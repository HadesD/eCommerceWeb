<template>
    <a-spin :spinning="loadingProductList">
        <div ref="productListRow">
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

            <a-row :gutter="16">
                <a-col :sm="24" :md="12" :lg="6" v-for="product in products?.data" key="product.id" style="margin-bottom: 16px;">
                    <router-link
                        :to="{
                            name: 'product',
                            params: {
                                category_slug: $route.params.category_slug || product.categories[0].slug,
                                product_slug: product.slug,
                            }
                        }"
                    >
                        <a-card hoverable>
                            <template #cover>
                                <img
                                    alt="example"
                                    src="/favicon.ico"
                                />
                            </template>
                            <template class="ant-card-actions" #actions>
                                <!-- <setting-outlined key="setting" />
                                <edit-outlined key="edit" />
                                <ellipsis-outlined key="ellipsis" /> -->
                            </template>
                            <a-card-meta :title="number_format(product.price) + ' ₫'" :description="product.name">
                                <!-- <template #avatar>
                                    <a-avatar src="https://zos.alipayobjects.com/rmsportal/ODTLcjxAfvqbxHnVXCYX.png" />
                                </template> -->
                            </a-card-meta>
                        </a-card>
                    </router-link>
                </a-col>
            </a-row>

            <a-pagination
                v-model:current="products.current_page"
                :total="products.total"
                :pageSize="products.per_page"
                @change="(page) => loadProductList({ page })"
            />
        </div>
    </a-spin>
</template>
<script>
import { onMounted, ref, watch } from 'vue';

import RequestRepository from '../utils/RequestRepository';

import {
    number_format,
} from '../helpers';

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

        const loadProductList = ({ page, keyword }) => {
            loadingProductList.value = true;

            RequestRepository.get('/products', {
                params: {
                    page,
                    keyword,
                    category_slug: props.categorySlug,
                    sort_by: sortBy.value,
                    price_range: props.priceRange,
                },
            })
                .then(res => {
                    products.value = res.data;

                    // console.log(productListRow.value);
                    productListRow.value.scrollIntoView({behavior: 'smooth'});
                })
                .finally(() => {
                    loadingProductList.value = false;
                });
        };

        const onSortByChange = (value) => {
            sortBy.value = value;
        };

        onMounted(() => {
            loadProductList({});
        });

        watch(
            () => props.categorySlug,
            () => loadProductList({})
        );

        watch(
            () => props.keyword,
            () => loadProductList({})
        );

        watch(
            () => props.priceRange,
            () => loadProductList({})
        );

        watch(
            () => sortBy.value,
            () => loadProductList({})
        );

        return {
            productListRow,

            products,
            loadingProductList,

            sortBy,

            loadProductList,
            number_format,
            onSortByChange,
        };
    },
}
</script>
