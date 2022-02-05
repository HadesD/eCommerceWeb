<template>
    <a-modal
        title="Thêm chuyên mục mới"
        :visible="visible"
        @cancel="() => $emit('handleCancel')"
        :footer="false"
    >
        <a-form ref="ruleForm" :model="formData" :rules="rules" @finish="onFinish" layout="vertical">
            <a-form-item label="Tên chuyên mục" name="name">
                <a-input v-model:value="formData.name" @change="onNameChanged" />
            </a-form-item>
            <a-form-item label="Đường dẫn URL (Slug)" name="slug">
                <a-input v-model:value="formData.slug" />
            </a-form-item>
            <a-form-item label="Mô tả ngắn">
                <a-textarea v-model:value="formData.description" placeholder="Nhập mô tả" :auto-size="{ minRows: 3, maxRows: 5  }" />
            </a-form-item>
            <a-form-item label="Chuyên mục cha">
                <a-form-item :style="{ display: 'inline-block', width: 'calc(100% - 40px)' }" >
                    <a-spin :spinning="categoriesTreeLoading">
                        <a-tree-select
                            show-search
                            allow-clear
                            v-model:value="formData.parent_id"
                            tree-data-simple-mode
                            treeDefaultExpandAll
                            treeNodeFilterProp="title"
                            style="width: 100%"
                            :dropdown-style="{ maxHeight: '400px', overflow: 'auto' }"
                            :tree-data="categoriesTreeData"
                            placeholder="Chuyên mục cha"
                            :replaceFields="{ pId:'parent_id',title:'name',value:'id' }"
                        />
                    </a-spin>
                </a-form-item>
                <a-form-item :style="{ display: 'inline-block', float:'right' }">
                    <a-button type="primary" @click="reloadCategoriesTree" :loading="categoriesTreeLoading">
                        <template #icon><ReloadOutlined /></template>
                    </a-button>
                </a-form-item>
            </a-form-item>
            <a-form-item>
                <a-button type="primary" htmlType="submit" size="large" block>Tạo</a-button>
            </a-form-item>
        </a-form>
    </a-modal>
</template>
<script>
import { reactive, ref, } from 'vue';

import {
    ReloadOutlined,
} from '@ant-design/icons-vue';

import { vietnameseNormalize,  } from '~/helpers.js';
import RequestRepository from '~/dashboard/utils/RequestRepository';
import { showErrorRequestApi } from '../../helpers';

const TREE_ROOT_ID = 0;
const TREE_NONE_PARENT_ID = -1;

export default {
    props: {
        visible: Boolean,
        categories: Array,
        categoriesTreeLoading: Boolean,
    },
    components: {
        ReloadOutlined,
    },
    setup() {
        const ruleForm = ref();
        const formData = reactive({
            name: undefined,
            parent_id: TREE_ROOT_ID,
            slug: undefined,
            description: undefined,
        });

        return {
            ruleForm,
            formData,
        };
    },
    data() {
        return {
            confirmLoading: false,
            rules: {
                name: [
                    { required: true, trigger: 'blur' },
                ],
                slug: [
                    { required: true, trigger: 'blur' },
                ],
            },
        };
    },
    computed:{
        categoriesTreeData(){
            let data = [{
                id: TREE_ROOT_ID, parent_id: TREE_NONE_PARENT_ID, name: 'Không có'
            }].concat(this.categories);

            for (let i = 0; i < data.length; i++) {
                let cur = data[i];
                if (cur.parent_id === TREE_ROOT_ID) {
                    cur.parent_id = TREE_NONE_PARENT_ID;
                }
                cur.pId = cur.parent_id;
            }

            return data;
        },
    },
    methods: {
        onNameChanged(e){
            this.formData.slug = vietnameseNormalize(e.target.value);
        },
        reloadCategoriesTree(){
            this.$emit('updateCategories');
        },
        onFinish(e) {
            this.confirmLoading = true;

            RequestRepository.post('/categories', this.formData)
                .then(res => {
                    this.$emit('handleOk');

                    this.$message.success(`Tạo chuyên mục [${res.data.data.name}] thành công`);

                    this.formData.name = undefined;
                    this.formData.slug = undefined;
                    this.formData.description = undefined;

                    this.reloadCategoriesTree();
                })
                .catch(showErrorRequestApi)
                .then(()=>{
                    this.confirmLoading = false;
                });
        },
    },
};
</script>
