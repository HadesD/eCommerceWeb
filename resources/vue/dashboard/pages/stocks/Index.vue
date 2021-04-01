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
        <a-col :span="20" :lg="20" :md="24" :sm="24" :xs="24" :style="{borderLeft: (['xs','sm','md'].indexOf($mq) !== -1) ?  'none' : '1px solid #CCC'}">
            <a-page-header title="Kho hàng">
                <template slot="tags">
                    <a-tooltip title="Làm mới">
                        <a-button type="primary" icon="reload" :loading="stocksTableLoading" @click="() => loadStocks({})" />
                    </a-tooltip>
                </template>
                <template slot="extra">
                    <router-link to="/stocks/new">
                    <a-tooltip title="Nhập kho">
                        <a-button type="primary" icon="plus" style="float:right;" />
                    </a-tooltip>
                    </router-link>
                </template>
            </a-page-header>
            <a-table
                :columns="stocksTableColumns"
                :data-source="stocksTableData"
                :loading="stocksTableLoading"
                :row-key="record => record.id"
                :pagination="stocksTablePagination"
                @change="(pagination, filters) => {stocksTableFilters = filters;loadStocks({page: pagination.current});}"
            >
                <!-- Block Search: BEGIN -->
                <div
                    slot="filterSearchBox"
                    slot-scope="{ setSelectedKeys, selectedKeys, confirm, clearFilters, column }"
                    style="padding: 8px"
                >
                    <a-input
                        :placeholder="`Tìm ${column.title}`"
                        :value="selectedKeys[0]"
                        style="width: 188px; margin-bottom: 8px; display: block;"
                        @change="e => setSelectedKeys(e.target.value ? [e.target.value] : [])"
                        @pressEnter="() => $refs[`filterSearchBox.${column.dataIndex}.btn`].$el.click()"
                    />
                    <a-button
                        :ref="`filterSearchBox.${column.dataIndex}.btn`"
                        type="primary"
                        icon="search"
                        size="small"
                        style="width: 90px; margin-right: 8px"
                        @click="() => {confirm();}"
                    >Tìm</a-button>
                    <a-button size="small" style="width: 90px" @click="() => {setSelectedKeys([]);clearFilters();}">Reset</a-button>
                </div>
                <a-icon
                    slot="filterSearchBoxIcon"
                    slot-scope="filtered"
                    type="search"
                    :style="{ color: filtered ? '#108ee9' : undefined }"
                />
                <!-- Block Search: END -->

                <template slot="cost_price" slot-scope="value" style="display:block;text-align:right;">{{ number_format(value) }}</template>
                <template slot="quantity_info" slot-scope="record">
                    <div>Đã bán: {{ record.products.length }}</div>
                    <div>Tồn kho: {{ record.quantity }}</div>
                </template>
                <template slot="time" slot-scope="record">
                    <div>Nhập kho: {{ date_format(record.in_date) }}</div>
                    <div>Tạo: {{ date_format(record.created_at) }}</div>
                    <div>Update: {{ date_format(record.updated_at) }}</div>
                </template>
                <template slot="action" slot-scope="record">
                    <template v-if="!onFinishSelect">
                        <router-link :to="`/stocks/${record.id}/edit`">
                            <a-button type="primary" icon="edit" />
                        </router-link>
                        <a-popconfirm title="Chắc chưa?" @confirm="() => onDeleteConfirmed(record)">
                            <a-icon slot="icon" type="question-circle-o" style="color: red" />
                            <a-button type="danger" icon="delete" />
                        </a-popconfirm>
                    </template>
                    <template v-else>
                        <a-button
                            type="primary" icon="bank" @click="() => onFinishSelect(record)"
                            :disabled="record.quantity <= 0"
                        >Chọn</a-button>
                    </template>
                </template>
            </a-table>
        </a-col>
    </a-row>
</template>

<script>
import StockStatus from '../../configs/StockStatus';
import { number_format, date_format } from '../../../helpers';

const stocksTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
    },
    {
        title: 'Tên',
        dataIndex: 'name',
        scopedSlots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Id/Imei',
        dataIndex: 'idi',
        scopedSlots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Giá lúc nhập (VND)',
        dataIndex: 'cost_price',
        scopedSlots: {
            customRender: 'cost_price'
        },
    },
    {
        title: 'Số lượng',
        key: 'quantity_info',
        scopedSlots: { customRender: 'quantity_info' },
    },
    {
        title: 'Thời gian',
        key: 'time',
        scopedSlots: { customRender: 'time' },
    },
    {
        title: 'Người update',
        key: 'update_user_id',
        scopedSlots: { customRender: 'update_user' },
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

            stocks: [],
            stocksTableLoading: false,
            stocksTableColumns,
            stocksTablePagination: {
                position: 'both',
            },
            stocksTableFilters: {},
        };
    },
    mounted() {
        this.loadCategoriesTree();

        this.currentCategoryId = (parseInt(this.$route.query.category_id) || undefined);
        this.stocksTablePagination.current = (parseInt(this.$route.query.page) || 1);

        this.loadStocks({});
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

        stocksTableData(){
            return this.stocks;
        },
        configStockStatus(){
            return StockStatus;
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
            this.stocksTablePagination.current = 1;

            this.loadStocks({});
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

        loadStocks({category_id, page}) {
            this.stocksTableLoading = true;

            axios.get('/api/stocks', {
                params: {
                    category_id: category_id || this.currentCategoryId,
                    page: page || this.stocksTablePagination.current,
                    ...this.stocksTableFilters,
                },
            })
                .then(res => {
                    const resData = res.data;
                    this.stocks = resData.data || [];

                    const newPagi = {
                        total: resData.total,
                        current: resData.current_page,
                        pageSize: resData.per_page,
                    };
                    this.stocksTablePagination = {...newPagi};

                    //   if ((this.$route.query.page != resData.current_page) || (this.$route.query.category_id != category_id)) {
                    //     this.$router.push('/stocks/index?page='+resData.current_page+'&category_id='+category_id);
                    //   }
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.stocksTableLoading = false;
                });
        },

        onDeleteConfirmed(record){
            return axios.delete(`/api/stocks/${record.id}`)
                .then(res => {
                    this.$message.success('Xóa thành công');

                    this.loadStocks({});
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Xóa thất bại');
                })
                .finally(()=>{
                });
        },
    },
}
</script>
