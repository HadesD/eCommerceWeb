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
                    :selectedKeys="[currentCategoryId]"
                />
            </a-spin>
        </a-col>
        <a-col :span="20" :lg="20" :md="24" :sm="24" :xs="24" :style="{borderLeft: (['xs','sm','md'].indexOf($grid.breakpoint) !== -1) ?  'none' : '1px solid #CCC'}">
            <a-page-header title="Sản phẩm">
                <template #tags>
                    <a-tooltip title="Làm mới">
                        <a-button type="primary" :loading="productsTableLoading" @click="() => loadProducts()">
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
                    productsTablePagination = pagination;
                    loadProducts();
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

                <template #id="{ text, record }">
                    <div :title="`Tạo: ${date_format(record.created_at)}\nUpdate: ${date_format(record.updated_at)}`">{{ text }}</div>
                </template>

                <template #name="{ text, record }">
                    <div>{{ text }}</div>
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
            <ProductEdit :productId="currentProductId" @productUpdated="(productId) => loadProducts()" />
        </a-modal>
    </a-row>
</template>

<script>
import { useRouter } from 'vue-router';

import {
    SearchOutlined, DownloadOutlined, PlusOutlined,
    ReloadOutlined, ShoppingCartOutlined, EditOutlined,
    BankOutlined,
} from '@ant-design/icons-vue';

import ProductStatus, { Config as configProductStatus } from '~/configs/ProductStatus';
import { number_format, date_format, showErrorRequestApi, list_to_tree } from '~/helpers';
import RequestApiRepository from '~/utils/RequestApiRepository';

import ProductEdit from '~/dashboard/pages/products/Edit.vue';
import AddCategoryModal from '~/dashboard/components/AddCategoryModal.vue';
import { defineComponent } from '@vue/runtime-core';
import * as _ from 'lodash';

const productsTableColumns = [
    {
        title: '#ID',
        dataIndex: 'id',
        slots: {
            customRender: 'id',
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
        title: 'Hành động',
        key: 'action',
        slots: {
            customRender: 'action',
        },
    },
];

export default defineComponent({
    props: {
        onFinishSelect: Function,
    },
    components: {
        AddCategoryModal,

        ProductEdit,

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
        if (!this.isModalMode) {
            this.currentCategoryId = this.$route.query.category_id;
        }
        this.loadCategoriesTree();

        if (!this.isModalMode) {
            this.productsTablePagination.current = this.$route.query.page;

            this.productsTableFilters = this.$route.query;
            delete this.productsTableFilters.sort_by;
            delete this.productsTableFilters.page;
            delete this.productsTableFilters.category_id;

            this.productsTableSorts = this.$route.query.sort_by;
        }

        this.loadProducts();
    },
    computed: {
        /**
         * Is on modal / import mode
         * @return bool
         */
        isModalMode() {
            return this.onFinishSelect !== undefined;
        },
        categoriesTreeData() {
            this.categoriesTreeExpandedKeys = [];

            const cats = this.categories.map((v) => {
                this.categoriesTreeExpandedKeys.push(v.id);

                return {
                    key: v.id,
                    title: v.name,
                    parent_id: v.parent_id,
                };
            });

            return list_to_tree(cats, 'parent_id', 'key');
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
            RequestApiRepository.get('/categories')
                .then(res => {
                    this.categories = res.data;
                })
                .finally(() => {
                    this.categoriesTreeLoading = false;
                });
        },
        updateCategories(cats) {
            this.loadCategoriesTree();
        },
        onCategoriesTreeSelect(keys, event) {
            this.currentCategoryId = keys[0];
            this.productsTablePagination.current = 1;

            this.loadProducts();
        },
        onCategoriesTreeExpand(keys, e) {
            if (!e.expanded) {
                this.categoriesTreeExpandedKeys.splice(this.categoriesTreeExpandedKeys.indexOf(e.node.key), 1);
            } else {
                this.categoriesTreeExpandedKeys = keys;
            }
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
        loadProducts(){
            this.productsTableLoading = true;

            // Reset popup data
            this.currentProductId = undefined;

            const params = {
                category_id: this.currentCategoryId,
                page: this.productsTablePagination.current,
                ...this.productsTableFilters,
                sort_by: this.productsTableSorts,
                is_dashboard: true,
            };
            if (!this.isModalMode) {
                this.$router.replace({
                    query: params,
                });
            }

            RequestApiRepository.get('/products', {
                params,
            })
                .then(res => {
                    this.products = res.data || [];

                    this.productsTablePagination = {
                        ...this.productsTablePagination,
                        total: resData.total,
                        current: resData.current_page,
                        pageSize: resData.per_page,
                    };
                })
                .finally(() => {
                    this.productsTableLoading = false;
                });
        },
    },
});
</script>
