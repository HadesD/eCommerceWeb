<template>
    <a-row :gutter="16">
        <AddCategoryModal
        :visible="addCategoryModalVisible"
        :categories="categories"
        :categoriesTreeLoading="categoriesTreeLoading"
        @handleOk="addCategoryModalHandleOk"
        @handleCancel="addCategoryModalHandleCancel"
        @updateCategories="updateCategories"
        />
        <a-col :span="4" :lg="4" :md="24" :sm="24" :xs="24">
            <a-page-header title="Chuyên mục">
                <template slot="extra">
                    <a-tooltip title="Thêm chuyên mục">
                        <a-button type="primary" icon="plus" @click="showAddCategoryModal" style="float:right;" />
                    </a-tooltip>
                </template>
            </a-page-header>
            <a-spin :spinning="categoriesTreeLoading || productsTableLoading">
                <a-tree
                    show-line
                    :expandedKeys="categoriesTreeExpandedKeys"
                    :tree-data="categoriesTreeData"
                    @select="onCategoriesTreeSelect"
                    @expand="onCategoriesTreeExpand"
                />
            </a-spin>
        </a-col>
        <a-col :span="20" :lg="20" :md="24" :sm="24" :xs="24" :style="{borderLeft: (['xs','sm','md'].indexOf($mq) !== -1) ?  'none' : '1px solid #CCC'}">
            <a-page-header title="Sản phẩm">
                <template slot="tags">
                    <a-tooltip title="Làm mới">
                        <a-button type="primary" icon="reload" :loading="productsTableLoading" @click="() => loadProducts(currentCategoryId, productsTablePagination.current)" />
                    </a-tooltip>
                </template>
                <template slot="extra">
                    <router-link to="/products/new">
                        <a-tooltip title="Thêm sản phẩm">
                            <a-button type="primary" icon="plus" style="float:right;" />
                        </a-tooltip>
                    </router-link>
                </template>
            </a-page-header>
            <a-table
                :columns="productsTableColumns"
                :data-source="productsTableData"
                :loading="productsTableLoading"
                :row-key="record => record.id"
                :pagination="productsTablePagination"
                @change="(pagination) => loadProducts(currentCategoryId, pagination.current)"
            >
                <span slot="name" slot-scope="record">
                    {{ record.name }}<br />
                    <a-tag>{{ record.slug }}</a-tag>
                </span>
                <span slot="status" slot-scope="record">
                    <a-tag :color="configProductStatus[record.status].color">{{ configProductStatus[record.status].name }}</a-tag>
                </span>
                <span slot="price" slot-scope="record" style="display:block;text-align:right;">
                    {{ number_format(record.price) }}
                </span>
                <span slot="time" slot-scope="record">
                    <div>Tạo: {{ date_format(record.created_at) }}</div>
                    <div>Update: {{ date_format(record.updated_at) }}</div>
                </span>
                <span slot="action" slot-scope="record">
                <template v-if="!onFinishSelect">
                    <router-link :to="`/products/${record.id}/edit`">
                        <a-button type="primary" icon="edit">Sửa</a-button>
                    </router-link>
                    <a-popconfirm title="Chắc chưa?" @confirm="() => onDeleteConfirmed(record)">
                        <a-icon slot="icon" type="question-circle-o" style="color: red" />
                        <a-button type="danger" icon="delete">Xóa</a-button>
                    </a-popconfirm>
                </template>
                <template v-else>
                    <a-button
                        type="primary" icon="shopping-cart" @click="() => onFinishSelect(record)"
                        :disabled="record.status === ProductStatus.STS_SOLDOUT"
                    >Chọn</a-button>
                </template>
                </span>
            </a-table>
        </a-col>
    </a-row>
</template>

<script>
import ProductStatus, { Config as configProductStatus } from '../../configs/ProductStatus';
import { number_format, date_format } from '../../../helpers';

const productsTableColumns = [
  {
    title: '#',
    dataIndex: 'id',
    key: 'id',
  },
  {
    title: 'Tên',
    key: 'name',
    scopedSlots: { customRender: 'name' },
  },
  {
    title: 'Trạng thái',
    key: 'status',
    scopedSlots: { customRender: 'status' },
  },
  {
    title: 'Giá (VND)',
    key: 'price',
    scopedSlots: { customRender: 'price' },
  },
  {
    title: 'Thời gian',
    key: 'time',
    scopedSlots: { customRender: 'time' },
  },
  {
    title: 'Hành động',
    key: 'action',
    scopedSlots: { customRender: 'action' },
  },
];

export default {
    props: {
        onFinishSelect: Function,
    },
    components: {
        AddCategoryModal: () => import('../../components/AddCategoryModal.vue'),
    },
    data() {
        return {
            categories: [],
            addCategoryModalVisible: false,
            categoriesTreeLoading: false,
            currentCategoryId: 0,
            categoriesTreeExpandedKeys: [],

            products: [],
            productsTableLoading: false,
            productsTableColumns,
            productsTablePagination: {
                position: 'both',
            },
        };
    },
    mounted(){
        this.loadCategoriesTree();

        this.currentCategoryId = (parseInt(this.$route.query.category_id) || '');
        this.productsTablePagination.current = (parseInt(this.$route.query.page) || 1);

        this.loadProducts(this.currentCategoryId, this.productsTablePagination.current);
    },
    computed: {
        categoriesTreeData(){
            const getParent = (key, tree) => {
                let parent;
                for (let i = 0; i < tree.length; i++) {
                    const node = tree[i];
                    if (node.key === key) {
                        parent = node;
                    } else if (node.children && node.children.length) {
                        parent = getParent(key, node.children);
                    }
                }

                return parent;
            };

            const sortedCategories = this.categories;

            // Reset
            this.categoriesTreeExpandedKeys = [];

            let data = [];
            for (let i = 0; i < sortedCategories.length; i++) {
                const cur = sortedCategories[i];

                const newData = {
                key: cur.id,
                title: cur.name,
                children: [],
                };

                let parent = getParent(cur.parent_id, data);
                let toNode = parent ? parent.children : data;
                toNode.push(newData);

                toNode = toNode.sort((a,b) => a.title.toUpperCase() > b.title.toUpperCase() ? 1 : -1);

                // Update key
                this.categoriesTreeExpandedKeys.push(newData.key);
            }

            return data;
        },
        productsTableData(){
            return this.products;
        },

        ProductStatus() {
            return ProductStatus;
        },
        configProductStatus() {
            return configProductStatus;
        },
    },
    methods: {
        number_format,
        date_format,
        // CategoriesTree
        loadCategoriesTree(){
            this.categoriesTreeLoading = true;
            axios.get('/api/categories')
                .then(res => {
                    this.categories = res.data.data.sort((a, b) => a.parent_id - b.parent_id);
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
        onCategoriesTreeSelect(keys, event) {
            this.currentCategoryId = keys[0];

            this.loadProducts(this.currentCategoryId, 1);
        },
        onCategoriesTreeExpand() {
            console.log('Trigger Expand');
        },

        // Modal
        showAddCategoryModal() {
            this.addCategoryModalVisible = true;
        },
        addCategoryModalHandleOk(e){
            // this.addCategoryModalVisible = false;
        },
        addCategoryModalHandleCancel(e){
            this.addCategoryModalVisible = false;
        },

        // Product
        loadProducts(category_id, page){
            this.productsTableLoading = true;

            axios.get('/api/products', {
                params: {
                    page, category_id,
                },
            })
                .then(res => {
                const resData = res.data;
                this.products = resData.data || [];

                const newPagi = {
                    total: resData.total,
                    current: resData.current_page,
                    pageSize: resData.per_page,
                };
                this.productsTablePagination = {...newPagi};

                //   if ((this.$route.query.page != resData.current_page) || (this.$route.query.category_id != category_id)) {
                //     this.$router.push('/products/index?page='+resData.current_page+'&category_id='+category_id);
                //   }
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Load thất bại');
                })
                .finally(()=>{
                    this.productsTableLoading = false;
                });
        },

        onDeleteConfirmed(record){
            axios.delete(`/api/products/${record.id}`)
                .then(res => {
                    this.$message.success('Xóa thành công');

                    this.loadProducts(this.currentCategoryId, this.productsTablePagination.current);
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error('Xóa thất bại');
                })
                .finally(()=>{
                });
        },
    },
}
</script>
