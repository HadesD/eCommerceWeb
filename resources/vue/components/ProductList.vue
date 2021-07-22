<template>
    <a-spin :spinning="loadingProductList">
        <a-row :gutter="16">
            <a-col :span="6" v-for="product in products?.data" key="product.id" style="margin-bottom: 16px;">
                  <a-card hoverable>
                    <template #cover>
                        <img
                            alt="example"
                            src="https://gw.alipayobjects.com/zos/rmsportal/JiqGstEfoWAOHiTxclqi.png"
                        />
                    </template>
                    <template class="ant-card-actions" #actions>
                        <setting-outlined key="setting" />
                        <edit-outlined key="edit" />
                        <ellipsis-outlined key="ellipsis" />
                    </template>
                    <a-card-meta :title="number_format(product.price) + ' ₫'" :description="product.name">
                        <!-- <template #avatar>
                            <a-avatar src="https://zos.alipayobjects.com/rmsportal/ODTLcjxAfvqbxHnVXCYX.png" />
                        </template> -->
                    </a-card-meta>
                </a-card>
            </a-col>
        </a-row>

        <a-pagination
            v-model:current="products.current_page"
            :total="products.total"
            :pageSize="products.per_page"
            @change="(page) => loadProductList({ page })"
        />
    </a-spin>
</template>
<script>
import { onMounted, ref, watch } from 'vue'

import RequestRepository from '../utils/RequestRepository';

import {
    number_format,
} from '../helpers';

export default {
    props: {
        categorySlug: String,
    },

    setup(props) {
        const products = ref({});
        const loadingProductList = ref(false);

        const loadProductList = ({ page }) => {
            loadingProductList.value = true;
            RequestRepository.get('/products', {
                params: {
                    page,
                    category_slug: props.categorySlug,
                },
            })
                .then(res => {
                    products.value = res.data;
                })
                .finally(() => {
                    loadingProductList.value = false;
                });
        };

        onMounted(() => {
            loadProductList({});
        });

        watch(
            () => props.categorySlug,
            () => loadProductList({})
        );

        return {
            products,
            loadingProductList,
            loadProductList,

            number_format,
        };
    },
}
</script>
