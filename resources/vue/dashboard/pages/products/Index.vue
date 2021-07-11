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
                <template #extra>
                    <a-tooltip title="Thêm chuyên mục">
                        <a-button type="primary" @click="showAddCategoryModal">
                            <template #icon><PlusOutlined /></template>
                        </a-button>
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
        <a-col :span="20" :lg="20" :md="24" :sm="24" :xs="24" :style="{borderLeft: (['xs','sm','md'].indexOf($grid.breakpoint) !== -1) ?  'none' : '1px solid #CCC'}">
            <a-page-header title="Sản phẩm">
                <template #tags>
                    <a-tooltip title="Làm mới">
                        <a-button type="primary" :loading="productsTableLoading" @click="() => loadProducts({})">
                            <template #icon><ReloadOutlined /></template>
                        </a-button>
                    </a-tooltip>
                </template>
                <template #extra>
                    <a-tooltip title="Thêm sản phẩm">
                        <a-button type="primary" @click="() => { currentProductId = undefined; productEditPageVisible = true; }">
                            <template #icon><PlusOutlined /></template>
                        </a-button>
                    </a-tooltip>
                </template>
            </a-page-header>
                <!-- :scroll="(['xs','sm','md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300, y: '85vh' } : {}" -->
            <a-table
                :size="['xs','sm','md'].indexOf($grid.breakpoint) !== -1 ? 'small' : 'default'"
                :columns="productsTableColumns"
                :data-source="productsTableData"
                :loading="productsTableLoading"
                :row-key="record => record.id"
                :pagination="productsTablePagination"
                @change="(pagination, filters, sorter) => {
                    productsTableFilters = filters;
                    productsTableSorts = (sorter.column && sorter.columnKey) ? ((sorter.order === 'descend' ? '-' : '+') + sorter.columnKey) : undefined;
                    loadProducts({page: pagination.current});
                }"
            >
                <!-- Block Search: BEGIN -->
                <template #filterSearchBox="{ setSelectedKeys, selectedKeys, confirm, clearFilters, column }">
                    <div style="padding: 8px">
                        <a-input
                            :placeholder="`Tìm ${column.title}`"
                            :value="selectedKeys[0]"
                            style="width: 188px; margin-bottom: 8px; display: block;"
                            @change="e => setSelectedKeys(e.target.value ? [e.target.value] : [])"
                            @pressEnter="() => $refs[`filterSearchBoxSubmit.${column.dataIndex}`].$el.click()"
                        />
                        <a-button
                            :ref="`filterSearchBoxSubmit.${column.dataIndex}`"
                            type="primary"
                            size="small"
                            style="width: 90px; margin-right: 8px"
                            @click="() => {confirm();}"
                        ><template #icon><SearchOutlined /></template> Tìm</a-button>
                        <a-button size="small" style="width: 90px" @click="() => {setSelectedKeys([]);clearFilters();}">Reset</a-button>
                    </div>
                </template>
                <template #filterSearchBoxIcon="{ filtered }">
                    <SearchOutlined :style="{ color: filtered ? '#108ee9' : undefined }" />
                </template>
                <!-- Block Search: END -->

                <template #name="{ text, record }">
                    <div>{{ text }}</div>
                    <a-tag>{{ record.slug }}</a-tag>
                </template>
                <template #status="{ text }">
                    <a-tag v-if="configProductStatus[text]" :color="configProductStatus[text].color">{{ configProductStatus[text].name }}</a-tag>
                </template>
                <template #price="{ text }">
                    <div style="display:block;text-align:right;">{{ number_format(text) }} ₫</div>
                </template>
                <template #categories="{ text }">
                    <a-tag v-for="category in text" :key="category.id">{{ category.name }}</a-tag>
                </template>
                <template #time="{ record }">
                    <div>Tạo: {{ date_format(record.created_at) }}</div>
                    <div>Update: {{ date_format(record.updated_at) }}</div>
                </template>
                <template #action="{ record }">
                    <template v-if="!onFinishSelect">
                        <a-button type="primary" @click="() => { currentProductId = record.id; productEditPageVisible = true; }">
                            <template #icon><EditOutlined /></template>
                        </a-button>
                    </template>
                    <template v-else>
                        <a-button
                            type="primary" @click="() => onFinishSelect(record)"
                            :disabled="record.status === ProductStatus.STS_SOLDOUT"
                        >
                            <template #icon><ShoppingCartOutlined /></template> Chọn
                        </a-button>
                    </template>
                </template>
            </a-table>
        </a-col>
        <a-modal
            :visible="productEditPageVisible"
            @cancel="() => productEditPageVisible = false"
            :footer="false"
            width="98vw"
        >
            <ProductEdit :productId="currentProductId" />
        </a-modal>
    </a-row>
</template>

<script>
import { defineAsyncComponent } from 'vue';

import {
    SearchOutlined, DownloadOutlined, PlusOutlined,
    ReloadOutlined, ShoppingCartOutlined, EditOutlined,
    BankOutlined,
} from '@ant-design/icons-vue';

import ProductStatus, { Config as configProductStatus } from '../../configs/ProductStatus';
import { number_format, date_format } from '../../../helpers';
import RequestRepository from '../../utils/RequestRepository';

const productsTableColumns = [
    {
        title: '#ID',
        dataIndex: 'id',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Tên',
        dataIndex: 'name',
        slots: {
            customRender: 'name',
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Trạng thái',
        dataIndex: 'status',
        slots: { customRender: 'status' },
        filters: Object.keys(configProductStatus).map(value => {
            return {
                text: configProductStatus[value].name,
                value,
            };
        }),
    },
    {
        title: 'Giá bán',
        dataIndex: 'price',
        slots: {
            customRender: 'price',
        },
        sorter: true,
    },
    {
        title: 'Chuyên mục',
        dataIndex: 'categories',
        slots: {
            customRender: 'categories',
        },
    },
    {
        title: 'Thời gian',
        key: 'time',
        slots: {
            customRender: 'time',
        },
    },
    {
        title: 'Hành động',
        key: 'action',
        slots: {
            customRender: 'action',
        },
    },
];

export default {
    props: {
        onFinishSelect: Function,
    },
    components: {
        AddCategoryModal: defineAsyncComponent(() => import('../../components/AddCategoryModal.vue')),
        ProductEdit: defineAsyncComponent(() => import('../products/Edit')),

        SearchOutlined, DownloadOutlined, PlusOutlined,
        ReloadOutlined, ShoppingCartOutlined, EditOutlined,
        BankOutlined,
    },
    data() {
        return {
            productEditPageVisible: false,
            currentProductId: undefined,

            categories: [],
            addCategoryModalVisible: false,
            categoriesTreeLoading: false,
            currentCategoryId: 0,
            categoriesTreeExpandedKeys: [],

            edittingId: undefined,

            products: [],
            productsTableLoading: false,
            productsTableColumns,
            productsTablePagination: {
                position: 'both',
            },
            productsTableFilters: {},
            productsTableSorts: undefined,

            ProductStatus,
            configProductStatus,
        };
    },
    mounted(){
        this.loadCategoriesTree();

        this.currentCategoryId = (parseInt(this.$route.query.category_id) || undefined);
        this.productsTablePagination.current = 1;

        this.loadProducts({});
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

    },
    methods: {
        number_format,
        date_format,

        // CategoriesTree
        loadCategoriesTree() {
            this.categoriesTreeLoading = true;
            RequestRepository.get('/categories')
                .then(res => {
                    this.categories = res.data.data.sort((a, b) => a.parent_id - b.parent_id);
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
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
            this.productsTablePagination.current = 1;

            this.loadProducts({});
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
        loadProducts({page}){
            this.productsTableLoading = true;

            // Reset popup data
            this.currentProductId = undefined;

            RequestRepository.get('/products', {
                params: {
                    category_id: this.currentCategoryId,
                    page: page || this.productsTablePagination.current,
                    ...this.productsTableFilters,
                    sort_by: this.productsTableSorts,
                },
            })
                .then(res => {
                    const resData = res.data;

                    this.products = resData.data || [];

                    this.productsTablePagination = {
                        ...this.productsTablePagination,
                        total: resData.total,
                        current: resData.current_page,
                        pageSize: resData.per_page,
                    };

                    //   if ((this.$route.query.page != resData.current_page) || (this.$route.query.category_id != category_id)) {
                    //     this.$router.push('/products/index?page='+resData.current_page+'&category_id='+category_id);
                    //   }
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Load thất bại');
                })
                .finally(()=>{
                    this.productsTableLoading = false;
                });
        },
    },
}
</script>
