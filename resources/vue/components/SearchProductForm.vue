<template>
    <a-form @finish="onSearch">
        <a-form-item name="search_text">
            <a-input
                size="large"
                allow-clear
                v-model:value="formState.search_text"
                required
                placeholder="Nhập từ khoá sản phẩm muốn tìm kiếm..."
            >
                <template #suffix>
                    <a-button type="primary" htmlType="submit"><SearchOutlined /></a-button>
                </template>
            </a-input>
        </a-form-item>
     </a-form>
</template>
<script>
import { ref, reactive } from 'vue';
import { useRouter } from 'vue-router';

import {
    SearchOutlined,
} from '@ant-design/icons-vue';

export default {
    components: {
        SearchOutlined,
    },
    setup() {
        const router = useRouter();

        const formState = reactive({
            search_text: undefined,
        });

        const onSearch = value => {
            console.log(value);
            router.push({
                name: 'search',
                query: {
                    keyword: value,
                },
            });
        };

        return {
            formState,

            onSearch,
        };
    },
}
</script>
