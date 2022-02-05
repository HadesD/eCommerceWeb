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
            <a-spin :spinning="categoriesTreeLoading || stocksTableLoading">
                <a-tree
                    show-line
                    :expandedKeys="categoriesTreeExpandedKeys"
                    :tree-data="categoriesTreeData"
                    @select="onCategoriesTreeSelect"
                    @expand="onCategoriesTreeExpand"
                >
                </a-tree>
            </a-spin>
        </a-col>
        <a-col :span="20" :lg="20" :md="24" :sm="24" :xs="24" :style="{borderLeft: (['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ?  '1px solid #CCC' : 'none'}">
            <a-page-header title="Kho hàng">
                <template #tags>
                    <a-tooltip title="Làm mới">
                        <a-button type="primary" :loading="stocksTableLoading" @click="() => loadStocks()">
                            <template #icon><ReloadOutlined /></template>
                        </a-button>
                    </a-tooltip>
                </template>
                <template #extra>
                    <a-tooltip title="Tải CSV">
                        <a-button type="primary" :disabled="stocks.length <= 0" @click="() => download()">
                            <template #icon><DownloadOutlined /></template>
                        </a-button>
                    </a-tooltip>
                    <a-tooltip title="Nhập kho">
                        <a-button type="primary" @click="() => { currentStockId = null; stockEditPageVisible = true; }">
                            <template #icon><PlusOutlined /></template>
                        </a-button>
                    </a-tooltip>
                </template>
            </a-page-header>
                <!-- :size="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? 'small' : 'default'" -->
            <a-table
                :scroll="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300, y: '85vh' } : {}"
                :columns="stocksTableColumns"
                :data-source="stocksTableData"
                :loading="stocksTableLoading"
                :row-key="record => record.id"
                :pagination="stocksTablePagination"
                @change="(pagination, filters, sorter) => {
                    stocksTableFilters = filters;
                    stocksTableSorts = (sorter.column && sorter.columnKey) ? ((sorter.order === 'descend' ? '-' : '+') + sorter.columnKey) : null;
                    stocksTablePagination = pagination;
                    loadStocks();
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
                        >
                            <template #icon><SearchOutlined /></template> Tìm
                        </a-button>
                        <a-button size="small" style="width: 90px" @click="() => {setSelectedKeys([]);clearFilters();}">Reset</a-button>
                    </div>
                </template>
                <template #filterSearchBoxIcon="{ filtered }">
                    <SearchOutlined :style="{ color: filtered ? '#108ee9' : undefined }" />
                </template>
                <!-- Block Search: END -->

                <template #price="{ text }">
                    <div style="text-align:right;">{{ number_format(text || 0) }} ₫</div>
                </template>
                <template #quantity="{ text }">
                    <a-tag :color="(text > 0) ? 'green' : 'red'">{{ text }}</a-tag>
                </template>
                <template #categories="{ text }">
                    <a-tag v-for="category in text" :key="category.id">{{ category.name }}</a-tag>
                </template>
                <template #tester="{ text, record }">
                    <div v-if="text && record.tester">
                        <div>
                            <span>#{{ text }}. {{ record.tester.name }}</span>
                            <a-button @click="() => { currentUserId = text; userEditPageVisible = true; }" size="small">
                                <template #icon><SearchOutlined /></template>
                            </a-button>
                        </div>
                        <div>Phone: {{ record.tester.phone || 'Chưa có' }}</div>
                    </div>
                </template>
                <template #update_info="{ record }">
                    <div>Tạo: {{ date_format(record.created_at) }}</div>
                    <div>Update: {{ date_format(record.updated_at) }}</div>
                    <div v-if="record.updated_user">
                        <span>Cuối bởi: {{ record.updated_user.name }}</span>
                        <a-button @click="() => { currentUserId = record.updated_user_id; userEditPageVisible = true; }" size="small">
                            <template #icon><SearchOutlined /></template>
                        </a-button>
                    </div>
                </template>
                <template #action="{ record }">
                    <template v-if="!onFinishSelect">
                        <a-button type="primary" @click="() => { currentStockId = record.id; stockEditPageVisible = true; }">
                            <template #icon><EditOutlined /></template>
                        </a-button>
                    </template>
                    <template v-else>
                        <a-button
                            type="primary" @click="() => onFinishSelect(record)"
                            :disabled="record.quantity <= 0"
                        >
                            <template #icon><BankOutlined /></template> Chọn
                        </a-button>
                    </template>
                </template>
            </a-table>
        </a-col>

        <a-modal
            :visible="stockEditPageVisible"
            @cancel="() => stockEditPageVisible = false"
            :footer="false"
            width="98vw"
        >
            <StockEdit :stockId="currentStockId" />
        </a-modal>

        <a-modal
            :visible="userEditPageVisible"
            @cancel="() => userEditPageVisible = false"
            :footer="false"
            :width="800"
        >
            <UserEdit :userId="currentUserId" />
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

import { number_format, date_format, defineAsyncComponent, showErrorRequestApi } from '~/helpers';
import RequestRepository from '~/dashboard/utils/RequestRepository';
import AddCategoryModal from '~/dashboard/components/AddCategoryModal.vue';

import UserEdit from '~/dashboard/pages/users/Edit.vue';
import StockEdit from '~/dashboard/pages/stocks/Edit.vue';
import { defineComponent } from '@vue/runtime-core';

const stocksTableColumns = [
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
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Id/Imei',
        dataIndex: 'idi',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Giá nhập (Đơn giá)',
        dataIndex: 'cost_price',
        slots: {
            customRender: 'price',
        },
        sorter: true,
    },
    {
        title: 'Giá bán dự định (Đơn giá)',
        dataIndex: 'sell_price',
        slots: {
            customRender: 'price',
        },
        sorter: true,
    },
    {
        title: 'Người kiểm thử',
        dataIndex: 'tester_id',
        slots: {
            customRender: 'tester',
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Tồn kho',
        dataIndex: 'quantity',
        slots: {
            customRender: 'quantity',
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

        UserEdit,
        StockEdit,

        SearchOutlined, DownloadOutlined, PlusOutlined,
        ReloadOutlined, ShoppingCartOutlined, EditOutlined,
        BankOutlined,
    },

    data() {
        return {
            stockEditPageVisible: false,
            currentStockId: null,

            userEditPageVisible: false,
            currentUserId: null,

            categories: [],
            addCategoryModalVisible: false,
            categoriesTreeLoading: false,
            currentCategoryId: 0,
            categoriesTreeExpandedKeys: [],

            stocks: [],
            stocksTableLoading: false,
            stocksTableColumns,
            stocksTablePagination: {
                position: 'both',
            },
            stocksTableFilters: {},
            stocksTableSorts: null,
        };
    },
    mounted() {
        if (!this.isModalMode) {
            this.currentCategoryId = this.$route.query.category_id;
        }
        this.loadCategoriesTree();

        if (!this.isModalMode) {
            this.stocksTablePagination.current = this.$route.query.page;

            this.stocksTableFilters = this.$route.query;
            delete this.stocksTableFilters.sort_by;
            delete this.stocksTableFilters.page;
            delete this.stocksTableFilters.category_id;

            this.stocksTableSorts = this.$route.query.sort_by;
        }

        this.loadStocks();
    },
    computed: {
        /**
         * Is on modal / import mode
         * @return bool
         */
        isModalMode() {
            return this.onFinishSelect !== undefined;
        },

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

        stocksTableData(){
            return this.stocks;
        },
    },
    methods: {
        number_format,
        date_format,
        // CategoriesTree
        loadCategoriesTree(){
            this.categoriesTreeLoading = true;
            RequestRepository.get('/categories')
                .then(res => {
                    this.categories = res.data.data.sort((a, b) => a.parent_id - b.parent_id);
                })
                .catch(showErrorRequestApi)
                .finally(()=>{
                    this.categoriesTreeLoading = false;
                });
        },
        updateCategories(cats) {
            this.loadCategoriesTree();
        },
        onCategoriesTreeSelect(keys, event) {
            this.currentCategoryId = keys[0];
            this.stocksTablePagination.current = 1;

            this.loadStocks();
        },

        onCategoriesTreeExpand() {
            console.log('Trigger Expand');
        },

        // Modal
        showAddCategoryModal() {
            this.addCategoryModalVisible = true;
        },
        addCategoryModalHandleOk(e) {
            // this.addCategoryModalVisible = false;
        },
        addCategoryModalHandleCancel(e) {
            this.addCategoryModalVisible = false;
        },

        loadStocks() {
            const router = useRouter();

            this.stocksTableLoading = true;

            // Reset popup data
            this.currentUserId = null;
            this.currentStockId = null;

            const params = {
                category_id: this.currentCategoryId,
                page: this.stocksTablePagination.current,
                ...this.stocksTableFilters,
                sort_by: this.stocksTableSorts,
            };

            if (!this.isModalMode) {
                this.$router.replace({
                    query: params,
                });
            }

            RequestRepository.get('/stocks', {
                params,
            })
                .then(res => {
                    const resData = res.data;

                    this.stocks = resData.data || [];

                    this.stocksTablePagination = {
                        ...this.stocksTablePagination,
                        total: resData.total,
                        current: resData.current_page,
                        pageSize: resData.per_page,
                    };
                })
                .catch(showErrorRequestApi)
                .finally(()=>{
                    this.stocksTableLoading = false;
                });
        },

        download() {
            const filters = this.stocksTableFilters;
            const downloadUrl = new URL(window.location.href);
            downloadUrl.pathname = RequestRepository.defaults.baseURL + '/stocks';
            downloadUrl.searchParams.append('download', 'csv');
            if (this.stocksTableSorts) {
                downloadUrl.searchParams.append('sort_by', this.stocksTableSorts);
            }
            if (this.currentCategoryId) {
                downloadUrl.searchParams.append('category_id', this.currentCategoryId);
            }
            Object.keys(filters).forEach(value => {
                const filterVal = filters[value];
                if (Array.isArray(filterVal)) {
                    filterVal.forEach(aVal => {
                        downloadUrl.searchParams.append(`${value}[]`, aVal);
                    });
                } else {
                    downloadUrl.searchParams.append(value, filters[value]);
                }
            });
            window.open(downloadUrl.href, '_blank');
        },

    },
});
</script>
