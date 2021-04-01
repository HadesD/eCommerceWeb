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
            />
            <a-form-model
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? {span: 4} : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? {span: 14} : {}"
            >
                <a-form-model-item label="Tên sản phẩm" ref="name" prop="name">
                    <a-input
                        @change="onNameChanged"
                        v-model="formData.name"
                        @blur="() => {$refs.name.onFieldBlur();$refs.slug.onFieldBlur()}"
                    />
                </a-form-model-item>
                <a-form-model-item label="Đường dẫn URL (Slug)" ref="slug" prop="slug">
                    <a-input
                        v-model="formData.slug"
                        @blur="() => $refs.slug.onFieldBlur()"
                    />
                </a-form-model-item>
                <a-form-model-item label="Giá bán" ref="price" prop="price"
                    :help="`VND: ${number_format(formData.price || 0)}`"
                >
                    <a-input-number
                        v-model="formData.price"
                        @blur="() => $refs.price.onFieldBlur()"
                        style="width: 100%;"
                        :min="0"
                        :max="2000000000"
                    />
                </a-form-model-item>
                <a-form-model-item label="Trạng thái" ref="status" prop="status">
                    <a-select
                        v-model="formData.status"
                        @blur="() => $refs.status.onFieldBlur()"
                    >
                        <a-select-option v-for="codeSts in Object.keys(configProductStatus)" :key="codeSts" :value="parseInt(codeSts)">{{ configProductStatus[codeSts].name }}</a-select-option>
                    </a-select>
                </a-form-model-item>
                <a-form-model-item label="Chuyên mục cha" ref="categories_id" prop="categories_id">
                    <a-form-model-item style="display: inline-block; margin-right: 5px;">
                        <a-tooltip title="Thêm chuyên mục">
                            <a-button type="primary" icon="plus" @click="showAddCategoryModal" />
                        </a-tooltip>
                    </a-form-model-item>
                    <a-form-model-item :style="{ display: 'inline-block', width: 'calc(100% - 80px)' }">
                        <a-spin :spinning="categoriesTreeLoading">
                            <a-tree-select
                                show-search
                                allow-clear
                                multiple
                                tree-data-simple-mode
                                treeDefaultExpandAll
                                treeNodeFilterProp="title"
                                v-model="formData.categories_id"
                                style="width: 100%"
                                :dropdown-style="{ maxHeight: '400px', overflow: 'auto' }"
                                :tree-data="categoriesTreeData"
                                placeholder="Chuyên mục"
                                :replaceFields="{ pId:'parent_id',title:'name',value:'id' }"
                                @blur="() => $refs.categories_id.onFieldBlur()"
                                @change="() => $refs.categories_id.onFieldBlur()"
                            />
                        </a-spin>
                    </a-form-model-item>
                    <a-form-model-item style="display: inline-block; margin-left: 5px;">
                        <a-tooltip title="Làm mới">
                            <a-button type="primary" icon="reload" @click="reloadCategoriesTree" :loading="categoriesTreeLoading" />
                        </a-tooltip>
                    </a-form-model-item>
                </a-form-model-item>
                <a-tabs default-active-key="description" @change="(k) => $refs[k] && $refs[k].focus()">
                    <a-tab-pane key="description" tab="Mô tả ngắn">
                        <a-form-model-item label="Mô tả ngắn">
                            <a-textarea
                                v-model="formData.description"
                                ref="description"
                                placeholder="Nhập mô tả"
                                :auto-size="{ minRows: 3, maxRows: 10 }"
                            />
                        </a-form-model-item>
                    </a-tab-pane>
                    <a-tab-pane key="detail" tab="Chi tiết sản phẩm">
                        <a-form-model-item label="Mô tả ngắn">
                            <a-textarea
                                v-model="formData.detail"
                                ref="detail"
                                placeholder="Nhập mô tả"
                                :auto-size="{ minRows: 3, maxRows: 10 }"
                            />
                        </a-form-model-item>
                    </a-tab-pane>
                    <a-tab-pane key="specification" tab="Thông số sản phẩm">
                        <a-form-model-item label="Mô tả ngắn">
                            <a-textarea
                                v-model="formData.specification"
                                ref="specification"
                                placeholder="Nhập mô tả"
                                :auto-size="{ minRows: 3, maxRows: 10 }"
                            />
                        </a-form-model-item>
                    </a-tab-pane>
                </a-tabs>
                <a-form-model-item :wrapper-col="{ span: 14, offset: 4 }">
                    <a-button
                        type="primary" htmlType="submit" @click="() => $refs.ruleForm.validate(valid => { if (valid) onFinish() })"
                    >{{ id ? 'Sửa' : 'Đăng bán' }}</a-button>
                </a-form-model-item>
            </a-form-model>
        </a-spin>
    </div>
</template>
<script>
import ProductStatus, { Config as configProductStatus } from '../../configs/ProductStatus';
import { vietnameseNormalize, number_format } from '../../../helpers';

export default {
    props: {
        productId: Number,
    },
    components: {
        AddCategoryModal: () => import('../../components/AddCategoryModal.vue'),
    },
    data() {
        return {
            categoriesTreeLoading: false,
            addCategoryModalVisible: false,
            categories: [],

            productInfoLoading: false,
            formData: {
                id: undefined,
                name: undefined,
                slug: undefined,
                categories_id: [],
                description: undefined,
                detail: undefined,
                specification: undefined,
                price: undefined,
                status: ProductStatus.STS_DRAFT,
            },
            rules: {
                name: [
                    { required: true },
                ],
                slug: [
                    { required: true },
                ],
                price: [
                    { required: true },
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
            return (this.productId !== undefined) ? this.productId : this.$route.params.id;
        },

        categoriesTreeData(){
            let data = this.categories;

            for (let i = 0; i < data.length; i++) {
                data[i].pId = data[i].parent_id;
            }

            return data;
        },
    },
    watch: {
        id() {
            this.loadProduct(this.id);
        },
    },
    mounted(){
        this.reloadCategoriesTree();

        this.loadProduct(this.id)
    },
    methods: {
        number_format,
        loadCategoriesTree(){
            this.reloadCategoriesTree();
        },
        reloadCategoriesTree(){
            this.categoriesTreeLoading = true;
            axios.get('/api/categories')
                .then(res => {
                    this.categories = res.data.data || [];
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
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
            if (!id) {
                return;
            }

            this.productInfoLoading = true;
            axios.get(`/api/products/${id}`)
                .then(res => {
                    const pData = res.data.data;
                    if (!pData.id) {
                        throw res;
                    }

                    _.assign(this.formData, _.pick(pData, _.keys(this.formData)));

                    this.formData.categories_id = pData.categories.map((item) => item.id);

                    this.productInfoLoading = false;
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
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
            axios({
                url: '/api/products' + (productId ? `/${productId}` : ''),
                method: productId ? 'put' : 'post',
                data: {
                    ...this.formData,
                },
            })
                .then(res => {
                    this.formData.id = res.data.data.id;

                    if (!this.formData.id) {
                        throw res;
                    }

                    if (productId) {
                        this.$message.success('Đã sửa sản phẩm thành công');
                    } else {
                        this.$message.success('Đã thêm sản phẩm thành công');

                        if (this.productId === undefined) {
                            this.$router.push({ path: `/products/${this.formData.id}/edit` });
                        }
                    }
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.productInfoLoading = false;
                });
        },
    },
};
</script>
