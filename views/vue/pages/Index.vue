<template>
    <div v-for="category in categories" :key="category.id" :id="`index-cat-${category.slug}`">
        <a-card
            v-if="catsProducts[category.slug]?.loading || catsProducts[category.slug]?.products?.length"
            style="margin-bottom: 15px;"
            size="small"
            :loading="catsProducts[category.slug]?.loading"
            :headStyle="{backgroundColor: 'blueviolet',borderLeft: '8px solid #f08b20'}"
        >
            <template #title>
                <a-divider orientation="left" style="margin: 0;">
                    <a-typography-title :level="2" style="margin: 0;">
                        <router-link style="color:#f7a21b;" :to="{
                            name: 'category',
                            params: {
                                category_slug: category.slug,
                            },
                        }">
                            <ShoppingOutlined /> {{ category.name }}
                        </router-link>
                    </a-typography-title>
                </a-divider>
            </template>
            <a-list
                :grid="{ gutter: 16, xs: 1, sm: 2, md: 4, lg: 4, xl: 6, xxl: 8, xxxl: 10}"
                :data-source="catsProducts[category.slug]?.products"
                :pagination="{
                    pageSize: 8,
                }"
            >
                <template #renderItem="{ item: product, }">
                    <a-list-item>
                        <router-link
                            :to="{
                                name: 'product',
                                params: {
                                    category_slug: category.slug,
                                    product_slug: vietnameseNormalize(product.name),
                                    product_id: product.id,
                                },
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
                    </a-list-item>
                </template>
            </a-list>
        </a-card>
    </div>
    <a-row :gutter="16">
        <a-col :xs="24" :sm="24" :md="12" :lg="6">
            <a-card style="margin-bottom: 10px;">
                <a-card-meta title="Miễn phí vận chuyển" description="Đơn hàng từ 300k">
                    <template #avatar>
                        <DollarCircleFilled style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :xs="24" :sm="24" :md="12" :lg="6">
            <a-card style="margin-bottom: 10px;">
                <a-card-meta title="Miễn phí đổi trả" description="Trong vòng 7 ngày">
                    <template #avatar>
                        <TransactionOutlined style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :xs="24" :sm="24" :md="12" :lg="6">
            <a-card style="margin-bottom: 10px;">
                <a-card-meta title="Bảo hành dài hạn" description="Lên tới 12 tháng">
                    <template #avatar>
                        <ClockCircleOutlined style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :xs="24" :sm="24" :md="12" :lg="6">
            <a-card style="margin-bottom: 10px;">
                <a-card-meta title="Tặng kèm hấp dẫn" description="Quà tặng đa dạng">
                    <template #avatar>
                        <GiftOutlined style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
    </a-row>
</template>
<script>
import { useStore } from 'vuex';
import {
    DollarCircleFilled, TransactionOutlined,
    ClockCircleOutlined, GiftOutlined,
    ShoppingOutlined,
} from '@ant-design/icons-vue';
import { computed, onActivated, onMounted, reactive, ref, toRefs, watch } from '@vue/runtime-core';

import app from '~/configs/app';
import RequestRepository from '~/utils/RequestRepository';
import {
    vietnameseNormalize,
    money_format,
} from '../helpers';

export default {
    components: {
        DollarCircleFilled, TransactionOutlined,
        ClockCircleOutlined, GiftOutlined,
        ShoppingOutlined,
    },

    setup() {
        const store = useStore();

        const catsProducts = reactive({
            /*
            ${catSlug}: {
                products: [],
                loading: Boolean,
            }
            */
        });
        const categories = computed(() => store.getters.getCategories);

        const loadCatsProducts = () => {
            categories.value.forEach(cat => {
                if (cat.parent_id) {
                    return;
                }
                const cp = catsProducts[cat.slug] = reactive({
                    products: [],
                    loading: true,
                });

                RequestRepository.get('/products', {
                    params: {
                        category_slug: cat.slug,
                    },
                })
                    .then(data => {
                        cp.products = data.data.data;
                    })
                    .catch(err => {

                    })
                    .finally(() => (cp.loading = false));
            });
        };

        watch(() => categories.value, () => loadCatsProducts());

        onMounted(() => {
            document.title = app.title;

            loadCatsProducts();

            if (window.FB && window.FB.XFBML) {
                window.FB.XFBML.parse();
            }
        });

        return {
            categories,
            catsProducts,

            vietnameseNormalize,
            money_format,

            lll: (a) => {console.log(a)}
        };
    },
}
</script>
