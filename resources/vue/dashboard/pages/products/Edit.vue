<template>
    <div>
        <AddCategoryModal
            :visible="addCategoryModalVisible"
            :categories="categories"
            :categoriesTreeLoading="categoriesTreeLoading"
            @handleOk="addCategoryModalHandleOk"
            @handleCancel="addCategoryModalHandleCancel"
            @updateCategories="updateCategories"
        />
        <a-spin :spinning="productInfoLoading">
            <a-page-header
                :title="id ? `Sản phẩm: ${formData.name}` : 'Đăng bán sản phẩm mới'"
                :sub-title="id ? `#${id}` : false"
            >
                <template #tags>
                    <a-tooltip title="Lấy dữ liệu mới nhất" v-if="id">
                        <a-button type="primary" size="small" :loading="productInfoLoading" @click="() => loadProduct(id)">
                            <template #icon><ReloadOutlined /></template>
                        </a-button>
                    </a-tooltip>
                    <a-tooltip title="Xoá toàn bộ dữ liệu đang nhập" v-if="!productId">
                        <a-popconfirm title="Xác nhận reset toàn bộ dữ liệu đang nhập?" @confirm="() => this.formData.id = (this.formData.id === undefined) ? null : undefined">
                            <a-button type="primary" danger size="small">
                                <template #icon><DeleteOutlined /></template>
                            </a-button>
                        </a-popconfirm>
                    </a-tooltip>
                </template>
                <a-descriptions size="small" :column="1" v-if="id">
                    <a-descriptions-item label="Ngày tạo">
                        <span>{{ date_format(productInfo.created_at) }}</span>
                    </a-descriptions-item>
                    <a-descriptions-item label="Ngày cập nhật">
                        <span>{{ date_format(productInfo.updated_at) }}</span>
                    </a-descriptions-item>
                </a-descriptions>
            </a-page-header>
            <a-form
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                @finish="onFinish"
                :label-col="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) === -1) ? {span: 4} : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) === -1) ? {span: 14} : {}"
            >
                <a-form-item label="Tên sản phẩm" name="name">
                    <a-input @change="onNameChanged" v-model:value="formData.name" />
                </a-form-item>
                <a-form-item label="Đường dẫn URL (Slug)" name="slug">
                    <a-input v-model:value="formData.slug" />
                </a-form-item>
                <a-form-item label="Giá bán" name="price" :help="`Xem trước: ${number_format(formData.price || 0)} ₫`">
                    <a-input-number
                        v-model:value="formData.price"
                        style="width: 100%;"
                        :min="0"
                        :max="2000000000"
                    />
                </a-form-item>
                <a-form-item label="Trạng thái" name="status">
                    <a-select v-model:value="formData.status">
                        <a-select-option v-for="codeSts in Object.keys(configProductStatus)" :key="codeSts" :value="parseInt(codeSts)">{{ configProductStatus[codeSts].name }}</a-select-option>
                    </a-select>
                </a-form-item>
                <a-form-item label="Chuyên mục cha">
                    <a-form-item style="display: inline-block; margin-right: 5px;">
                        <a-tooltip title="Thêm chuyên mục">
                            <a-button type="primary" @click="showAddCategoryModal">
                                <template #icon><PlusOutlined /></template>
                            </a-button>
                        </a-tooltip>
                    </a-form-item>
                    <a-form-item name="categories_id" :style="{ display: 'inline-block', width: 'calc(100% - 80px)' }">
                        <a-spin :spinning="categoriesTreeLoading">
                            <a-tree-select
                                show-search
                                allow-clear
                                multiple
                                tree-data-simple-mode
                                treeDefaultExpandAll
                                treeNodeFilterProp="title"
                                v-model:value="formData.categories_id"
                                style="width: 100%"
                                :dropdown-style="{ maxHeight: '400px', overflow: 'auto' }"
                                :tree-data="categoriesTreeData"
                                placeholder="Chuyên mục"
                                :replaceFields="{ pId:'parent_id',title:'name',value:'id' }"
                            />
                        </a-spin>
                    </a-form-item>
                    <a-form-item style="display: inline-block; margin-left: 5px;">
                        <a-tooltip title="Làm mới">
                            <a-button type="primary" @click="reloadCategoriesTree" :loading="categoriesTreeLoading">
                                <template #icon><ReloadOutlined /></template>
                            </a-button>
                        </a-tooltip>
                    </a-form-item>
                </a-form-item>
                <a-tabs default-active-key="description" @change="(k) => $refs[k] && $refs[k].focus()">
                    <a-tab-pane key="description" tab="Mô tả ngắn">
                        <a-form-item label="Mô tả ngắn">
                            <a-textarea
                                v-model:value="formData.description"
                                ref="description"
                                placeholder="Nhập mô tả"
                                :auto-size="{ minRows: 3, maxRows: 10 }"
                            />
                        </a-form-item>
                    </a-tab-pane>
                    <a-tab-pane key="detail" tab="Chi tiết sản phẩm">
                        <a-form-item label="Mô tả ngắn">
                            <a-textarea
                                v-model:value="formData.detail"
                                ref="detail"
                                placeholder="Nhập mô tả"
                                :auto-size="{ minRows: 3, maxRows: 10 }"
                            />
                        </a-form-item>
                    </a-tab-pane>
                    <a-tab-pane key="specification" tab="Thông số sản phẩm">
                        <a-form-item label="Mô tả ngắn">
                            <a-textarea
                                v-model:value="formData.specification"
                                ref="specification"
                                placeholder="Nhập mô tả"
                                :auto-size="{ minRows: 3, maxRows: 10 }"
                            />
                        </a-form-item>
                    </a-tab-pane>
                </a-tabs>
                <a-form-item :label-col="{ span: 0 }" :wrapper-col="{ span: 16, offset: (['xs','sm','md'].indexOf($grid.breakpoint) !== -1) ? 0 : 4 }">
                    <a-button type="primary" htmlType="submit" block>{{ id ? 'Sửa' : 'Đăng bán' }}</a-button>
                </a-form-item>
            </a-form>
        </a-spin>
    </div>
</template>
<script>
import { defineAsyncComponent, reactive, ref } from 'vue';

import {
    PlusOutlined, ReloadOutlined, DeleteOutlined,
} from '@ant-design/icons-vue';

import ProductStatus, { Config as configProductStatus } from '../../configs/ProductStatus';
import { vietnameseNormalize, number_format, date_format } from '../../../helpers';
import RequestRepository from '../../utils/RequestRepository';
import { message } from 'ant-design-vue';

export default {
    props: {
        productId: Number,
    },
    components: {
        AddCategoryModal: defineAsyncComponent(() => import('../../components/AddCategoryModal.vue')),

        PlusOutlined, ReloadOutlined, DeleteOutlined,
    },

    setup() {
        const ruleForm = ref();
        const formData = reactive({
                id: undefined,
                name: undefined,
                slug: undefined,
                categories_id: [],
                description: undefined,
                detail: undefined,
                specification: undefined,
                price: undefined,
                status: ProductStatus.STS_DRAFT,
            });

        return {
            ruleForm,
            formData,
        };
    },

    data() {
        return {
            categoriesTreeLoading: false,
            addCategoryModalVisible: false,
            categories: [],

            productInfo: {},
            productInfoLoading: false,
            rules: {
                name: [
                    { required: true },
                ],
                slug: [
                    { required: true },
                ],
                price: [
                    { required: true },
                    { type: 'integer' },
                ],
                status: [
                    { required: true },
                ],
                categories_id: [
                    { required: true },
                ],
            },

            ProductStatus,
            configProductStatus,
        };
    },
    computed:{
        id() {
            return this.productId || this.formData.id;
        },

        categoriesTreeData() {
            let data = this.categories;

            for (let i = 0; i < data.length; i++) {
                data[i].pId = data[i].parent_id;
            }

            return data;
        },
    },
    watch: {
        productId() {
            this.formData.id = undefined;
        },

        id(to) {
            if (to) {
                this.loadProduct(to);
            } else {
                this.$refs.ruleForm.resetFields();

                this.productInfo = {};
            }
        },
    },
    mounted(){
        this.reloadCategoriesTree();

        if (this.id) {
            this.loadProduct(this.id);
        }
    },
    methods: {
        number_format,
        date_format,

        loadCategoriesTree(){
            this.reloadCategoriesTree();
        },
        reloadCategoriesTree(){
            this.categoriesTreeLoading = true;
            RequestRepository.get('/categories')
                .then(res => {
                    this.categories = res.data.data || [];
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        message.error(err.response.data.message);
                        return;
                    }

                    message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.categoriesTreeLoading = false;
                });
        },
        updateCategories(cats) {
            this.loadCategoriesTree();
        },
        showAddCategoryModal() {
            this.addCategoryModalVisible = true;
        },
        addCategoryModalHandleOk(e){
            // this.addCategoryModalVisible = false;
        },
        addCategoryModalHandleCancel(e){
            this.addCategoryModalVisible = false;
        },

        loadProduct(id){
            this.productInfoLoading = true;

            RequestRepository.get(`/products/${id}`)
                .then(res => {
                    const pData = res.data.data;
                    if (!pData.id) {
                        throw res;
                    }

                    this.productInfo = pData;

                    _.assign(this.formData, _.pick(pData, _.keys(this.formData)));

                    this.formData.categories_id = pData.categories.map((item) => item.id);

                    this.productInfoLoading = false;
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        message.error(err.response.data.message);
                        return;
                    }

                    message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                });
        },

        onNameChanged(e){
            this.formData.slug = vietnameseNormalize(e.target.value);
        },
        onFinish() {
            this.productInfoLoading = true;

            const productId = this.id;
            const request = productId ? RequestRepository.put : RequestRepository.post;
            request('/products' + (productId ? `/${productId}` : ''), {
                ...this.formData,
            })
                .then(res => {
                    this.formData.id = res.data.data.id;

                    if (!this.formData.id) {
                        throw res;
                    }

                    message.success(productId ? 'Đã sửa sản phẩm thành công' : 'Đã thêm sản phẩm thành công');

                    this.$emit('productUpdated', productId);

                    this.loadProduct(this.formData.id);
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        message.error(err.response.data.message);
                        return;
                    }

                    message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.productInfoLoading = false;
                });
        },
    },
};
</script>
