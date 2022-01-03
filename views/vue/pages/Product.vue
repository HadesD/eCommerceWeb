<template>
    <a-row :gutter="16">
        <a-col :xs="24" :sm="24" :md="24" :lg="12">
            <a-carousel arrows dots-class="slick-dots slick-thumb" class="product-carousel" dot-position="left">
                <template #customPaging="props">
                    <img :src="product.images[props.i]?.url" />
                </template>
                <div v-for="image in product.images" :key="image">
                    <img :src="image.url" :alt="product.name" />
                </div>
            </a-carousel>
        </a-col>
        <a-col :xs="24" :sm="24" :md="24" :lg="12">
            <a-skeleton active :paragraph="{ rows: 6 }" v-if="!product.id" />
            <div v-else>
                <a-space direction="vertical">
                    <a-typography-text v-for="category in product.categories">
                        <router-link
                            :to="{
                                name: 'category',
                                params: {
                                    category_slug: category.slug,
                                }
                            }"
                        >{{ category.name }}</router-link>
                    </a-typography-text>
                </a-space>
                <a-typography-title :level="2" style="margin-bottom: 0;">{{ product.name }}</a-typography-title>
                <a-typography-text type="secondary">Mã sản phẩm: <a-tag color="purple">#{{ product.id }}</a-tag></a-typography-text>
                <div><a-rate :value="4" disabled /></div>
                <a-divider dashed />
                <a-typography-paragraph :level="1">{{ product.description }}</a-typography-paragraph>
                <a-typography-title :level="2">Giá: <a-typography-text type="danger">{{ money_format(product.price) }}</a-typography-text></a-typography-title>
                <a-space>
                    <span>Số lượng:</span>
                    <a-input-number v-model:value="productAddCount" :min="1" size="large" />
                    <a-button @click="() => addToCart(productAddCount)" size="large" type="primary" ghost><ShoppingCartOutlined /> Thêm vào Giỏ Hàng</a-button>
                </a-space>
                <div style="margin-top: 15px;">
                    <a-button @click="() => buyProduct()" size="large" type="primary" :loading="buying">Mua</a-button>
                </div>
            </div>
        </a-col>
        <a-card title="Quá trình mua hàng" size="small" style="width: 100%; margin-top: 15px;">
            <a-steps :current="0">
                <a-step title="Thêm Vào Giỏ" description="Chọn số lượng muốn mua rồi nhấn nút Thêm Vào Giỏ Hàng" />
                <a-step title="Chốt Đơn" description="Tại trang Thanh Toán, kiểm tra lại thông tin đặt hàng và Chốt Đơn" />
                <a-step title="Thanh Toán" description="Thực hiện thanh toán thông qua các hình thức cửa hàng đang hỗ trợ" />
                <a-step title="Nhận Hàng" description="Sản phẩm được chuyển tới tận tay Quý Khách Hàng" />
            </a-steps>
        </a-card>
    </a-row>
</template>
<script>
import { onMounted, reactive, ref, watch, } from 'vue';
import { useRoute, useRouter } from 'vue-router'
import { useMeta } from 'vue-meta';

import {
    ShoppingCartOutlined,
} from '@ant-design/icons-vue';

import RequestRepository from '~/utils/RequestRepository';
import {
    vietnameseNormalize,
    money_format,
} from '~/helpers';
import { useStore } from 'vuex';

export default {
    components: {
        ShoppingCartOutlined,
    },

    setup() {
        const route = useRoute();
        const router = useRouter();
        const store = useStore();

        const product = ref({});
        const productAddCount = ref(1);
        const buying = ref(false);

        const loadProduct = () => {
            RequestRepository.get('/products/' + route.params.product_id)
                .then(res => {
                    const productData = res.data.data;

                    document.title = productData.name;

                    const productSlug = vietnameseNormalize(productData.name);
                    if (route.params.product_slug !== productSlug) {
                        router.replace({
                            params: {
                                ...route.params,
                                product_slug: productSlug,
                            },
                        });
                    }

                    product.value = productData;
                });
        };

        const addToCart = (num = 1) => {
            return store.dispatch('appendCartItem', {
                product,
                num,
            });
        };

        const buyProduct = () => {
            buying.value = true;
            addToCart(1)
                .then(() => {
                    router.push({
                        name: 'checkout',
                    });
                })
                .catch(() => {
                    buying.value = false;
                });
        };

        onMounted(() => {
            loadProduct();
        });

        watch(
            () => route.params.product_id,
            (value) => {
                if (value) {
                    loadProduct();
                }
            }
        );

        return {
            product,
            productAddCount,
            buying,

            addToCart,
            buyProduct,

            money_format,
        };
    },
}
</script>
