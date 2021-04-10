<template>
    <div>
        <a-page-header title="Giao dịch (Thu / Chi)">
            <template slot="tags">
                <a-tooltip title="Làm mới">
                    <a-button type="primary" icon="reload" :loading="transactionsTableLoading" @click="() => loadTransactions({})" />
                </a-tooltip>
            </template>
            <template slot="extra">
                <a-tooltip title="Tải CSV">
                    <a-button type="primary" icon="download" :disabled="transactions.length <= 0" @click="() => download()" />
                </a-tooltip>
            </template>
        </a-page-header>
        <a-table
            :columns="transactionsTableColumns"
            :data-source="transactionsTableData"
            :loading="transactionsTableLoading"
            :row-key="record => record.id"
            :pagination="transactionsTablePagination"
            @change="(pagination, filters, sorter) => {
                transactionsTableFilters = filters;
                transactionsTableSorts = (sorter.column && sorter.columnKey) ? ((sorter.order === 'descend' ? '-' : '+') + sorter.columnKey) : undefined;
                loadTransactions({page: pagination.current});
            }"
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
                    @pressEnter="() => $refs[`filterSearchBoxSubmit.${column.dataIndex}`].$el.click()"
                />
                <a-button
                    :ref="`filterSearchBoxSubmit.${column.dataIndex}`"
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

            <!-- Block Filter RangeDate: BEGIN -->
            <div
                slot="filterRangeDate"
                slot-scope="{ setSelectedKeys, confirm, clearFilters }"
                style="padding: 8px"
            >
                <a-range-picker
                    format="YYYY/MM/DD"
                    type="date"
                    style="width: 250px; margin-bottom: 8px; display: block;"
                    :ranges="{ 'Hôm nay': [moment(), moment()], 'Tháng này': [moment().startOf('month'), moment().endOf('month')] }"
                    @change="(date, dateStrings) => setSelectedKeys(dateStrings ? dateStrings : [])"
                />
                <a-button
                    type="primary"
                    icon="search"
                    size="small"
                    style="width: 90px; margin-right: 8px"
                    @click="() => {confirm();}"
                >Tìm</a-button>
                <a-button size="small" style="width: 90px" @click="() => {setSelectedKeys([]);clearFilters();}">Reset</a-button>
            </div>
            <!-- Block Filter RangeDate: END -->

            <template slot="description" slot-scope="value, record">
                <span>{{ value }}</span>
                <a-button v-if="record.order" size="small" icon="account-book" @click="() => { currentOrderId = record.order.id; orderEditPageVisible = true; }" />
                <a-button v-if="record.stock" size="small" icon="bank" @click="() => { currentStockId = record.stock.id; stockEditPageVisible = true; }" />
            </template>

            <template slot="amount" slot-scope="value">
                <div style="float:right;">
                    <a-tag :color="(value >= 0) ? 'green' : 'red'">{{ number_format(value) }}</a-tag>
                </div>
            </template>

            <template slot="cashier_id" slot-scope="value, record">
                <div v-if="record.cashier">
                    <span>#{{ value }}. {{ record.cashier.name }}</span>
                    <a-button icon="search" @click="() => { currentUserId = value; userEditPageVisible = true; }" size="small" />
                </div>
            </template>

            <template slot="time" slot-scope="record">
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
    </div>
</template>
<script>
import { number_format, date_format } from '../../../helpers';
import moment from 'moment';

const transactionsTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
    },
    {
        title: 'Nội dung',
        dataIndex: 'description',
        scopedSlots: {
            customRender: 'description',
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Số tiền',
        dataIndex: 'amount',
        scopedSlots: {
            customRender: 'amount',
        },
        sorter: true,
    },
    {
        title: 'Ngày thanh toán',
        dataIndex: 'paid_date',
        scopedSlots: {
            filterDropdown: 'filterRangeDate',
        },
        sorter: true,
    },
    {
        title: 'Thu ngân',
        dataIndex: 'cashier_id',
        scopedSlots: {
            customRender: 'cashier_id',
        },
    },
    {
        title: 'Thời gian',
        key: 'time',
        scopedSlots: {
            customRender: 'time',
        },
    },
];

export default {
    components: {
        StockEdit: () => import('../stocks/Edit'),
        OrderEdit: () => import('../orders/Edit'),
        UserEdit: () => import('../users/Edit'),
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
        this.loadTransactions({page: 1});
    },
    computed: {
        transactionsTableData() {
            return this.transactions;
        },
    },
    methods: {
        number_format,
        date_format,
        moment,

        loadTransactions({page}) {
            this.transactionsTableLoading = true;

            axios.get('/api/transactions', {
                params: {
                    page: page || this.transactionsTablePagination.current,
                    ...this.transactionsTableFilters,
                    sort_by: this.transactionsTableSorts,
                }
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
                    if (err.response && err.response.data.message) {
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
            downloadUrl.pathname = '/api/transactions';
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
