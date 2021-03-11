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
            <h2>{{ $route.params.id ? `Sửa hàng trong kho #${$route.params.id}` : 'Nhập hàng mới vào kho' }}</h2>
            <a-form-model
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? labelCol : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? wrapperCol : {}"
            >
                <a-form-model-item label="Tên sản phẩm" ref="name" prop="name">
                    <a-input
                        v-model="formData.name"
                    />
                </a-form-model-item>
                <a-form-model-item label="Id/imei/mã phân biệt" ref="idi" prop="idi">
                    <a-input
                        v-model="formData.idi"
                        @blur="() => $refs.idi.onFieldBlur()"
                    />
                </a-form-model-item>
                <a-form-model-item label="Số lượng" ref="quantity" prop="quantity">
                    <a-input-number
                        v-model="formData.quantity"
                        @blur="() => $refs.quantity.onFieldBlur()"
                        :min="-200"
                        :max="200"
                    >
                    </a-input-number>
                </a-form-model-item>
                <a-form-model-item label="Giá lúc nhập (Đơn giá)" ref="cost_price" prop="cost_price">
                    <a-input-number
                        v-model="formData.cost_price"
                        @blur="() => $refs.cost_price.onFieldBlur()"
                        style="width: 100%;"
                        :min="-2000000000"
                        :max="2000000000"
                        :disabled="cost_priceDisabled"
                    >
                    </a-input-number>
                    <span>VND: {{ new Intl.NumberFormat().format(formData.cost_price || 0) }}</span>
                </a-form-model-item>
                <a-form-model-item label="Chuyên mục cha" ref="categories_id" prop="categories_id">
                    <a-form-model-item
                        style="display: inline-block; margin-right: 5px;"
                    >
                        <a-tooltip title="Thêm chuyên mục">
                            <a-button type="primary" icon="plus" @click="showAddCategoryModal" />
                        </a-tooltip>
                    </a-form-model-item>
                    <a-form-model-item
                        :style="{ display: 'inline-block', width: 'calc(100% - 80px)' }"
                    >
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
                    <a-form-model-item
                        style="display: inline-block; margin-left: 5px;"
                    >
                        <a-tooltip title="Làm mới">
                            <a-button type="primary" icon="reload" @click="reloadCategoriesTree" :loading="categoriesTreeLoading" />
                        </a-tooltip>
                    </a-form-model-item>
                </a-form-model-item>
                <a-form-model-item label="Ghi chú">
                    <a-textarea
                        v-model="formData.note"
                    />
                </a-form-model-item>
                <a-form-model-item label="Ngày nhập" ref="in_date" prop="in_date">
                    <a-date-picker
                        v-model="formData.in_date"
                        format="YYYY-MM-DD HH:mm:ss"
                        show-time
                        type="date"
                        @blur="() => $refs.in_date.onFieldBlur()"
                    />
                </a-form-model-item>
                <a-form-model-item :wrapper-col="{ span: 14, offset: 4 }">
                    <a-button type="primary" @click="onSubmit">
                        {{ $route.params.id ? 'Sửa' : 'Nhập kho' }}
                    </a-button>
                    <a-button style="margin-left: 10px;" @click="resetForm">Reset</a-button>
                </a-form-model-item>
            </a-form-model>
        </a-spin>
    </div>
</template>

<script>
import axios from 'axios';

export default {
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
                id: undefined,
                name: '',
                idi: '',
                cost_price: 0,
                quantity: 1,
                note: '',
                in_date: undefined,
                addon_transactions: [],
                order_products: [],
                categories_id: [],
            },
            rules: {
                idi: [
                    { required: true, trigger: 'blur' },
                ],
                cost_price: [
                    { required: true, trigger: 'blur' },
                ],
                quantity: [
                    { required: true, trigger: 'blur' },
                ],
                categories_id: [
                    { required: true, trigger: 'blur' },
                ],
            },
        }
    },
    computed: {
        categoriesTreeData(){
            let data = this.categories;

            for (let i = 0; i < data.length; i++)
            {
                data[i].pId = data[i].parent_id;
            }

            return data;
        },
        cost_priceDisabled(){
            return (this.stockInfo.products && (this.stockInfo.products.length > 0));
        },
    },
    mounted() {
        this.formData.id = this.$route.params.id;

        this.reloadCategoriesTree();

        if (this.formData.id)
        {
            this.loadStock(this.formData.id)
        }
    },
    methods: {
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
                    console.log(err);

                    if (err.response && err.response.message) {
                        this.$message.error(err.response.message);
                        return;
                    }

                    this.$message.error('Thất bại');
                })
                .then(()=>{
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

        loadStock(id){
            this.stockInfoLoading = true;
            axios.get(`/api/stocks/${id}`)
                .then(res => {
                    const sData = res.data.data;
                    if (!sData.id)
                    {
                        throw res;
                        return;
                    }

                    _.assign(this.formData, _.pick(sData, _.keys(this.formData)));

                    this.formData.categories_id = sData.categories.map((item) => item.id);

                    this.stockInfo = sData;

                    this.stockInfoLoading = false;
                })
                .catch(err => {
                    console.log(err);

                    if (err.response && err.response.message) {
                        this.$message.error(err.response.message);
                        return;
                    }

                    this.$message.error('Thất bại');
                })
                .then(()=>{
                });
        },

        onSubmit() {
            this.$refs.ruleForm.validate(valid => {
                if (!valid)
                {
                    return false;
                }

                this.stockInfoLoading = true;

                const stockId = this.$route.params.id;
                if (stockId)
                {
                    axios.put(`/api/stocks/${stockId}`, this.formData)
                        .then(res => {
                            this.$message.success('Đã sửa sản phẩm thành công');
                        })
                        .catch(err => {
                            console.log(err);

                            if (err.response && err.response.message) {
                                this.$message.error(err.response.message);
                                return;
                            }

                            this.$message.error('Thất bại');
                        })
                        .then(()=>{
                            this.stockInfoLoading = false;
                        });
                }
                else
                {
                    axios.post('/api/stocks', this.formData)
                        .then(res => {
                            this.formData.id = res.data.data.id;

                            if (!this.formData.id)
                            {
                                throw res;
                                return;
                            }

                            this.$message.success('Đã thêm sản phẩm thành công');
                            this.$router.push({ path: `/stocks/${this.formData.id}/edit` });
                        })
                        .catch(err => {
                            console.log(err);

                            if (err.response && err.response.message) {
                                this.$message.error(err.response.message);
                                return;
                            }

                            this.$message.error('Thất bại');
                        })
                        .then(()=>{
                            this.stockInfoLoading = false;
                        });
                }
            });
        },
        resetForm() {
            this.$refs.ruleForm.resetFields();
        },
    },
}
</script>
