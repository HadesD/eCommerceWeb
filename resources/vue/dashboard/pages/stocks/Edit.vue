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
        <a-spin :spinning="stockInfoLoading">
            <a-page-header
                :title="id ? `Tên hàng: ${formData.name}` : 'Nhập hàng mới vào kho'"
                :sub-title="id ? `#${id}` : false"
            />
            <a-form-model
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? labelCol : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? wrapperCol : {}"
            >
                <a-form-model-item label="Tên sản phẩm" ref="name" prop="name">
                    <a-input v-model="formData.name" />
                </a-form-model-item>
                <a-form-model-item label="Id/imei/mã phân biệt" ref="idi" prop="idi">
                    <a-input v-model="formData.idi" @blur="() => $refs.idi.onFieldBlur()" />
                </a-form-model-item>
                <a-form-model-item label="Số lượng" ref="quantity" prop="quantity">
                    <a-input-number v-model="formData.quantity" @blur="() => $refs.quantity.onFieldBlur()" :min="-200" :max="200" />
                </a-form-model-item>
                <a-form-model-item label="Giá lúc nhập (Đơn giá)" ref="cost_price" prop="cost_price" :help="`VND: ${number_format(formData.cost_price || 0)}`">
                    <a-input-number
                        v-model="formData.cost_price"
                        @blur="() => $refs.cost_price.onFieldBlur()"
                        style="width: 100%;"
                        :min="-2000000000"
                        :max="2000000000"
                        :disabled="cost_priceDisabled"
                    />
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
                <a-form-model-item label="Ghi chú">
                    <a-textarea v-model="formData.note" />
                </a-form-model-item>
                <a-form-model-item label="Ngày nhập" ref="in_date" prop="in_date" help="Ngày nhập sẽ liên kết trực tiếp tới tiền vốn của tháng đó">
                    <a-date-picker
                        v-model="formData.in_date"
                        format="YYYY-MM-DD HH:mm:ss"
                        show-time
                        type="date"
                        @blur="() => $refs.in_date.onFieldBlur()"
                    />
                </a-form-model-item>
                <a-form-model-item :wrapper-col="{ span: 14, offset: 4 }" v-if="!stockId">
                    <a-button
                        type="primary" htmlType="submit" @click="() => $refs.ruleForm.validate((valid) => { if (valid) onFinish() })"
                    >{{ id ? 'Sửa' : 'Nhập kho' }}</a-button>
                </a-form-model-item>
            </a-form-model>
        </a-spin>
    </div>
</template>

<script>
import axios from 'axios';
import moment from 'moment';
import { number_format } from '../../../helpers';

export default {
    props: {
        stockId: Number,
    },
    components: {
        AddCategoryModal: () => import('../../components/AddCategoryModal.vue'),
    },
    data() {
        return {
            labelCol: { span: 4 },
            wrapperCol: { span: 14 },

            categoriesTreeLoading: false,
            addCategoryModalVisible: false,
            categories: [],

            stockInfoLoading: false,
            stockInfo: {},
            formData: {
                name: undefined,
                idi: undefined,
                cost_price: 0,
                quantity: 1,
                note: undefined,
                in_date: undefined,
                addon_transactions: [],
                order_products: [],
                categories_id: [],
            },
            rules: {
                idi: [
                    { required: true },
                ],
                cost_price: [
                    { required: true },
                ],
                quantity: [
                    { required: true },
                ],
                categories_id: [
                    { required: true },
                ],
                in_date: [
                    { required: true }
                ],
            },
        }
    },
    computed: {
        id() {
            return this.stockId || this.$route.params.id;
        },
        categoriesTreeData(){
            let data = this.categories;

            for (let i = 0; i < data.length; i++) {
                data[i].pId = data[i].parent_id;
            }

            return data;
        },
        cost_priceDisabled(){
            return (this.stockInfo.products && (this.stockInfo.products.length > 0));
        },
    },
    watch: {
        id(to, from) {
            this.loadStock(this.id);
        },
    },
    mounted() {
        this.reloadCategoriesTree();

        this.loadStock(this.id);
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
        addCategoryModalHandleOk(e) {
            // this.addCategoryModalVisible = false;
        },
        addCategoryModalHandleCancel(e) {
            this.addCategoryModalVisible = false;
        },

        loadStock(id) {
            if (!id) {
                return;
            }
            this.stockInfoLoading = true;
            axios.get(`/api/stocks/${id}`)
                .then(res => {
                    const sData = res.data.data;
                    if (!sData.id) {
                        throw res;
                    }

                    _.assign(this.formData, _.pick(sData, _.keys(this.formData)));

                    this.formData.categories_id = sData.categories.map((item) => item.id);
                    this.formData.in_date = moment(this.formData.in_date);

                    this.stockInfo = sData;

                    this.stockInfoLoading = false;
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

        onFinish() {
            this.stockInfoLoading = true;

            const stockId = this.id;

            axios({
                url: stockId ? `/api/stocks/${stockId}` : '/api/stocks',
                method: stockId ? 'put' : 'post',
                data: {
                    ...this.formData,
                    in_date: moment(this.formData.in_date).format('YYYY-MM-DD HH:mm:ss'),
                }
            })
                .then(res => {
                    const sData = res.data.data;
                    if (!sData.id) {
                        throw res;
                    }

                    if (stockId) {
                        this.$message.success('Đã sửa sản phẩm thành công');
                    } else {
                        this.$message.success('Đã thêm sản phẩm thành công');

                        this.$router.push({ path: `/stocks/${sData.id}/edit` });
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
                    this.stockInfoLoading = false;
                });
        },
    },
}
</script>
