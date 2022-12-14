<template>
    <a-form :rules="formRules" :model="formState" @finish="onSearch">
        <a-form-item name="search_text">
            <a-auto-complete
                :defaultActiveFirstOption="false"
                v-model:value="formState.search_text"
                :options="options"
                @change="onKeywordChange"
                @select="onSelect"
                @focus="onFocus"
            >
                <template #default>
                    <a-input
                        placeholder="Nhập từ khoá sản phẩm muốn tìm kiếm..."
                        allow-clear
                        @pressEnter="onPressEnter"
                    >
                        <template #suffix>
                            <a-button type="primary" htmlType="submit"><SearchOutlined /></a-button>
                        </template>
                    </a-input>
                </template>
            </a-auto-complete>
        </a-form-item>
     </a-form>
</template>
<script>
import { ref, reactive, onMounted, computed } from 'vue';
import { useRouter, useRoute } from 'vue-router';

import {
    SearchOutlined,
} from '@ant-design/icons-vue';
import { debounce } from 'lodash';

import { message } from 'ant-design-vue';

import RequestRepository from '~/utils/RequestRepository';
import {
    number_format,
    vietnameseNormalize,
} from '~/helpers';

export default {
    components: {
        SearchOutlined,
    },

    setup() {
        const router = useRouter();
        const route = useRoute();

        const formState = reactive({
            search_text: undefined,
        });
        const searchResults = ref([]);
        const options = computed(() => searchResults.value.map(value => {
            return {
                value: `${value.name} (${number_format(value.price)} ₫)`,
                id: value.id,
                name: value.name,
                slug: vietnameseNormalize(value.name),
                category_slug: value.categories[0]?.slug,
            };
        }));

        const formRules = {
            search_text: [
                {
                    required: true,
                },
            ],
        };

        const onSearch = value => {
            router.push({
                name: 'search',
                query: {
                    keyword: value.search_text,
                },
            });
        };
        const onSelect = (value, option) => {
            router.push({
                name: 'product',
                params: {
                    category_slug: option.category_slug,
                    product_slug: option.slug,
                    product_id: option.id,
                },
            });
            formState.search_text = option.name;
        };

        const onKeywordChange = debounce(value => {
            if (!value || !(value.length > 0)) {
                return;
            }
            RequestRepository.get('/products', {
                params: {
                    keyword: value,
                },
            })
                .then(res => {
                    searchResults.value = res.data.data;
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        message.error(err.response.data.message);
                        return;
                    }

                    message.error(err.message || 'Thất bại');
                })
        }, 300);

        const onPressEnter = () => {
            onSearch(formState);
        };

        const onFocus = () => {
            onKeywordChange(formState.search_text);
        };

        router.isReady()
            .then(() => {
                formState.search_text = route.query.keyword;
            });

        return {
            formState,
            formRules,
            options,

            onSearch,
            onSelect,
            onKeywordChange,
            onFocus,
            onPressEnter,
        };
    },
}
</script>
