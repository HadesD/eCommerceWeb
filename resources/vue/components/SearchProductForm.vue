<template>
    <a-form
        ref="formRef"
        @finish="onSearch"
    >
    <a-form-item name="search_text">
        <a-input
            size="large"
            v-model:value="formState.search_text"
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

import {
    SearchOutlined,
} from '@ant-design/icons-vue';

export default {
    components: {
        SearchOutlined,
    },
    setup() {
        const formRef = ref();
        const formState = reactive({
            search_text: undefined,
        });

        const onSearch = value => {

        };

        return {
            formRef,
            formState,

            onSearch,
        };
    },
}
</script>
