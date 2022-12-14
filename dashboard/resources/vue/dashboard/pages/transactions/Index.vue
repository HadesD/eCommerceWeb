<template>
    <a-page-header title="Giao dịch (Thu / Chi)">
        <template #tags>
            <a-tooltip title="Làm mới">
                <a-button type="primary" :loading="transactionsTableLoading" @click="() => loadTransactions()">
                    <template #icon>
                        <ReloadOutlined />
                    </template>
                </a-button>
            </a-tooltip>
        </template>
        <template #extra>
            <a-tooltip title="Tải CSV">
                <a-button type="primary" :disabled="transactions.length <= 0" @click="() => download()">
                    <template #icon>
                        <DownloadOutlined />
                    </template>
                </a-button>
            </a-tooltip>
        </template>
    </a-page-header>
        <!-- :scroll="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300, y: '85vh' } : {}" -->
    <a-table
        :size="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? 'small' : 'default'"
        :columns="transactionsTableColumns"
        :data-source="transactionsTableData"
        :loading="transactionsTableLoading"
        :row-key="record => record.id"
        :pagination="transactionsTablePagination"
        @change="(pagination, filters, sorter) => {
            transactionsTableFilters = filters;
            transactionsTableSorts = (sorter.column && sorter.columnKey) ? ((sorter.order === 'descend' ? '-' : '+') + sorter.columnKey) : undefined;
            transactionsTablePagination = pagination;
            loadTransactions();
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

        <!-- Block Filter RangeDate: BEGIN -->
        <template #filterRangeDate="{ setSelectedKeys, selectedKeys, confirm, clearFilters }">
            <div style="padding: 8px">
                <a-range-picker
                    format="YYYY/MM/DD"
                    type="date"
                    :value="selectedKeys"
                    style="width: 250px; margin-bottom: 8px; display: block;"
                    :ranges="{ 'Hôm nay': [dayjs(), dayjs()], 'Tháng này': [dayjs().startOf('month'), dayjs().endOf('month')] }"
                    @change="(date, dateStrings) => setSelectedKeys(dateStrings ? dateStrings : [])"
                />
                <a-button
                    type="primary"
                    size="small"
                    style="width: 90px; margin-right: 8px"
                    @click="() => {confirm();}"
                ><template #icon><SearchOutlined /></template> Tìm</a-button>
                <a-button size="small" style="width: 90px" @click="() => {setSelectedKeys([]);clearFilters();}">Reset</a-button>
            </div>
        </template>
        <!-- Block Filter RangeDate: END -->

        <template #type="{ text, record }">
            <a-tag>{{ text }}</a-tag>
            <a-button v-if="record.order" size="small" @click="() => { currentOrderId = record.order.id; orderEditPageVisible = true; }">
                <template #icon><AccountBookOutlined /></template>
            </a-button>
            <a-button v-if="record.stock" size="small" @click="() => { currentStockId = record.stock.id; stockEditPageVisible = true; }">
                <template #icon><BankOutlined /></template>
            </a-button>
        </template>

        <template #amount="{ text }">
            <div style="float:right;">
                <a-tag :color="(text >= 0) ? 'green' : 'red'">{{ number_format(text) }}</a-tag>
            </div>
        </template>

        <template #cashier_id="{ text, record }">
            <div v-if="record.cashier">
                <span>#{{ text }}. {{ record.cashier.name }}</span>
                <a-button @click="() => { currentUserId = text; userEditPageVisible = true; }" size="small">
                    <template #icon><SearchOutlined /></template>
                </a-button>
            </div>
        </template>

        <template #time="{ record }">
            <div>Tạo: {{ date_format(record.created_at) }}</div>
            <div>Update: {{ date_format(record.updated_at) }}</div>
        </template>
    </a-table>

    <a-modal
        :visible="stockEditPageVisible"
        @cancel="() => stockEditPageVisible = false"
        :footer="false"
        width="95vw"
    >
        <StockEdit :stockId="currentStockId" />
    </a-modal>

    <a-modal
        :visible="orderEditPageVisible"
        @cancel="() => orderEditPageVisible = false"
        :footer="false"
        width="95vw"
    >
        <OrderEdit :orderId="currentOrderId" />
    </a-modal>

    <a-modal
        :visible="userEditPageVisible"
        @cancel="() => userEditPageVisible = false"
        :footer="false"
        :width="800"
    >
        <UserEdit :userId="currentUserId" />
    </a-modal>
</template>

<script>
import dayjs from 'dayjs';

import {
    SearchOutlined, DownloadOutlined, PlusOutlined,
    ReloadOutlined, ShoppingCartOutlined, EditOutlined,
    AccountBookOutlined, BankOutlined,
} from '@ant-design/icons-vue';

import { number_format, date_format, defineAsyncComponent } from '~/helpers';
import RequestRepository from '~/dashboard/utils/RequestRepository';

const transactionsTableColumns = [
    {
        title: '#ID',
        dataIndex: 'id',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Loại giao dịch',
        dataIndex: 'type',
        slots: {
            customRender: 'type',
        },
    },
    {
        title: 'Nội dung',
        dataIndex: 'description',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Số tiền',
        dataIndex: 'amount',
        slots: {
            customRender: 'amount',
        },
        sorter: true,
    },
    {
        title: 'Ngày thanh toán',
        dataIndex: 'paid_date',
        slots: {
            filterDropdown: 'filterRangeDate',
        },
        sorter: true,
    },
    {
        title: 'Thu ngân',
        dataIndex: 'cashier_id',
        slots: {
            customRender: 'cashier_id',
        },
    },
    {
        title: 'Thời gian',
        key: 'time',
        slots: {
            customRender: 'time',
        },
    },
];

export default {
    components: {
        UserEdit: defineAsyncComponent(() => import('~/dashboard/pages/users/Edit.vue')),
        StockEdit: defineAsyncComponent(() => import('~/dashboard/pages/stocks/Edit.vue')),
        OrderEdit: defineAsyncComponent(() => import('~/dashboard/pages/orders/Edit.vue')),

        SearchOutlined, DownloadOutlined, PlusOutlined,
        ReloadOutlined, ShoppingCartOutlined, EditOutlined,
        AccountBookOutlined, BankOutlined,
    },
    data() {
        return {
            stockEditPageVisible: false,
            currentStockId: undefined,

            userEditPageVisible: false,
            currentUserId: undefined,

            orderEditPageVisible: false,
            currentOrderId: undefined,

            transactions: [],
            transactionsTableColumns,
            transactionsTableLoading: false,
            transactionsTablePagination: {
                position: 'both',
            },
            transactionsTableFilters: {},
            transactionsTableSorts: undefined,
        };
    },
    mounted() {
        if (!this.isModalMode) {
            this.transactionsTablePagination.current = this.$route.query.page;

            this.transactionsTableFilters = this.$route.query;
            delete this.transactionsTableFilters.sort_by;
            delete this.transactionsTableFilters.page;

            this.transactionsTableSorts = this.$route.query.sort_by;
        }

        this.loadTransactions();
    },
    computed: {
        /**
         * Is on modal / import mode
         * @return bool
         */
        isModalMode() {
            return this.onFinishSelect !== undefined;
        },

        transactionsTableData() {
            return this.transactions;
        },
    },
    methods: {
        number_format,
        date_format,
        dayjs,

        loadTransactions() {
            this.transactionsTableLoading = true;

            // Reset popup data
            this.currentUserId = undefined;
            this.currentStockId = undefined;
            this.currentOrderId = undefined;

            const params = {
                page: this.transactionsTablePagination.current,
                ...this.transactionsTableFilters,
                sort_by: this.transactionsTableSorts,
            };

            if (!this.isModalMode) {
                this.$router.replace({
                    query: params,
                });
            }

            RequestRepository.get('/transactions', {
                params
            })
                .then(res => {
                    const resData = res.data;

                    this.transactions = resData.data;

                    this.transactionsTablePagination = {
                        ...this.transactionsTablePagination,
                        total: resData.total,
                        current: resData.current_page,
                        pageSize: resData.per_page,
                    };
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(() => {
                    this.transactionsTableLoading = false;
                });
        },

        download() {
            const filters = this.transactionsTableFilters;
            const downloadUrl = new URL(window.location.href);
            downloadUrl.pathname = RequestRepository.defaults.baseURL + '/transactions';
            downloadUrl.searchParams.append('download', 'csv');
            if (this.transactionsTableSorts) {
                downloadUrl.searchParams.append('sort_by', this.transactionsTableSorts);
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
};
</script>
