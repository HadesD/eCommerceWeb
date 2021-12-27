<template>
    <a-page-header title="Hoá Đơn">
        <template #tags>
            <a-tooltip title="Làm mới">
                <a-button type="primary" :loading="ordersTableLoading" @click="() => loadOrders({})">
                    <template #icon>
                        <ReloadOutlined />
                    </template>
                </a-button>
            </a-tooltip>
        </template>
        <template #extra>
            <a-tooltip title="Tải CSV">
                <a-button type="primary" :disabled="orders.length <= 0" @click="() => download()">
                    <template #icon>
                        <DownloadOutlined />
                    </template>
                </a-button>
            </a-tooltip>
            <a-tooltip title="Thêm đơn">
                <a-button type="primary" @click="() => { currentOrderId = undefined; orderEditPageVisible = true; }">
                    <template #icon>
                        <PlusOutlined />
                    </template>
                </a-button>
            </a-tooltip>
        </template>
    </a-page-header>
        <!-- :scroll="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300, y: '85vh' } : {}" -->
    <a-table
        defaultExpandAllRows
        :size="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? 'small' : 'default'"
        :columns="ordersTableColumns"
        :data-source="ordersTableData"
        :loading="ordersTableLoading"
        :row-key="record => record.id"
        :pagination="ordersTablePagination"
        @change="(pagination, filters, sorter) => {
            ordersTableFilters = filters;
            ordersTableSorts = (sorter.column && sorter.columnKey) ? ((sorter.order === 'descend' ? '-' : '+') + sorter.columnKey) : undefined;
            ordersTablePagination = pagination;
            loadOrders();
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
                    :ranges="{ 'Hôm nay': [moment(), moment()], 'Tháng này': [moment().startOf('month'), moment().endOf('month')] }"
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

        <template #customer="{ text, record }">
            <div v-if="text && record.customer">
                <div>
                    <span>#{{ text }}. {{ record.customer.name }}</span>
                    <a-button @click="() => { currentUserId = text; userEditPageVisible = true; }" size="small">
                        <template #icon><SearchOutlined /></template>
                    </a-button>
                </div>
                <div>Phone: {{ record.customer.phone || 'Chưa có' }}</div>
            </div>
        </template>
        <template #status="{ record }">
            <a-tag v-if="configOrderStatus[record.status]" :color="configOrderStatus[record.status].color">{{ configOrderStatus[record.status].name }}</a-tag>
        </template>
        <template #total_amount="{ record }">
            <div>
                <span>Nhập: </span>
                <span>{{ calcCost(record) }} ₫</span>
            </div>
            <div>
                <span>Bán: </span>
                <span>{{ calcSell(record) }} ₫</span>
            </div>
            <div>
                <span>Thu Kho: </span>
                <span>{{ calcAmount(record) }} ₫</span>
            </div>
            <div>
                <span>GD Thêm: </span>
                <span>{{ calcAdd(record) }} ₫</span>
            </div>
        </template>
        <template #time="{ record }">
            <div>Tạo: {{ date_format(record.created_at) }}</div>
            <div>Update: {{ date_format(record.updated_at) }}</div>
            <div v-if="record.updated_user">
                <span>Cuối bởi: {{ record.updated_user.name }}</span>
                <a-button @click="() => { currentUserId = record.updated_user_id; userEditPageVisible = true; }" size="small">
                    <template #icon>
                        <SearchOutlined />
                    </template>
                </a-button>
            </div>
        </template>
        <template #action="{ record }">
            <template v-if="!onFinishSelect">
                <a-button type="primary" @click="() => { currentOrderId = record.id; orderEditPageVisible = true; }">
                    <template #icon>
                        <EditOutlined />
                    </template>
                </a-button>
            </template>
            <template v-else>
                <a-button type="primary" @click="() => onFinishSelect(record)">
                    <template #icon>
                        <ShoppingCartOutlined /> Chọn
                    </template>
                </a-button>
            </template>
        </template>

        <template #expandedRowRender="{ record: o }">
            <a-table
                v-if="o.order_products.length"
                defaultExpandAllRows
                :columns="orderProductTableColumns"
                :data-source="o.order_products"
                :pagination="false"
                :row-key="record => record.id"
                size="small"
                bordered
            >
                <template #expandedRowRender="{ record: op }">
                    <a-table
                        :columns="orderProductStockTableColumns"
                        :data-source="op.order_product_stocks"
                        :pagination="false"
                        :row-key="record => record.id"
                        size="small"
                        bordered
                    >
                    </a-table>
                </template>
            </a-table>
            <a-table
                v-if="o.transactions.length"
                :style="o.order_products.length ? 'margin-top: 25px;' : ''"
                :title="() => 'Giao dịch thêm'"
                :columns="transactionsTableColumns"
                :data-source="o.transactions"
                :pagination="false"
                :row-key="record => record.id"
                size="small"
                bordered
            >
            </a-table>
        </template>
    </a-table>

    <a-modal
        :visible="userEditPageVisible"
        @cancel="() => userEditPageVisible = false"
        :footer="false"
        :width="800"
    >
        <UserEdit :userId="currentUserId" />
    </a-modal>

    <a-modal
        :visible="stockEditPageVisible"
        @cancel="() => stockEditPageVisible = false"
        :footer="false"
        width="98vw"
    >
        <StockEdit :stockId="currentStockId" />
    </a-modal>

    <a-modal
        :visible="productEditPageVisible"
        @cancel="() => productEditPageVisible = false"
        :footer="false"
        width="98vw"
    >
        <ProductEdit :productId="currentProductId" />
    </a-modal>

    <a-modal
        :visible="orderEditPageVisible"
        @cancel="() => orderEditPageVisible = false"
        :footer="false"
        width="98vw"
    >
        <OrderEdit :orderId="currentOrderId" @orderUpdated="(orderId) => loadOrders()" />
    </a-modal>
</template>

<script>
import moment from 'moment';

import {
    SearchOutlined, DownloadOutlined, PlusOutlined,
    ReloadOutlined, ShoppingCartOutlined, EditOutlined,
} from '@ant-design/icons-vue';

import OrderStatus, { Config as configOrderStatus } from '~/configs/OrderStatus';
import OrderProductStockStatus from '~/configs/OrderProductStockStatus';
import { number_format, date_format, defineAsyncComponent } from '~/helpers';
import RequestRepository from '~/dashboard/utils/RequestRepository';

const ordersTableColumns = [
    {
        title: '#ID',
        dataIndex: 'id',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Trạng thái',
        key: 'status',
        slots: {
            customRender: 'status'
        },
        filters: Object.keys(configOrderStatus).map(value => {
            return {
                text: configOrderStatus[value].name,
                value,
            };
        }),
    },
    {
        title: 'Ngày xuất đơn',
        dataIndex: 'deal_date',
        slots: {
            filterDropdown: 'filterRangeDate',
            customRender: (text) => date_format(text),
        },
        sorter: true,
    },
    {
        title: 'Khách hàng',
        dataIndex: 'customer_id',
        slots: {
            customRender: 'customer',
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Tổng Tiền',
        key: 'total_amount',
        slots: {
            customRender: 'total_amount'
        },
    },
    {
        title: 'Ghi chú',
        dataIndex: 'note',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Thời gian',
        key: 'time',
        slots: {
            customRender: 'time'
        },
    },
    {
        title: 'Hành động',
        key: 'action',
        slots: {
            customRender: 'action'
        },
    },
];

const transactionsTableColumns = [
    {
        title: 'Nội dung',
        dataIndex: 'description',
    },
    {
        title: 'Số tiền',
        dataIndex: 'amount',
        customRender: (value) => number_format(value),
    },
    {
        title: 'Ngày thanh toán',
        dataIndex: 'paid_date',
    },
];

const orderProductTableColumns = [
    {
        title: 'Sản phẩm mua',
        key: 'name',
        // dataIndex: 'product',
        // customRender: ({ record }) => record.name,
        customRender: ({ record }) => record.product?.name,
    },
];

const orderProductStockTableColumns = [
    {
        title: 'Tên hàng xuất kho',
        key: 'name',
        // dataIndex: 'stock',
        customRender: ({ record }) => record.stock?.name,
    },
    {
        title: 'Idi/Imei',
        key: 'idi',
        // dataIndex: 'stock',
        customRender: ({ record }) => record.stock?.idi,
    },
    {
        title: 'Giá nhập',
        key: 'cost_price',
        // dataIndex: 'stock',
        customRender: ({ record }) => number_format(record.stock?.cost_price) + ' ₫',
    },
    {
        title: 'Giá bán',
        key: 'amount',
        // dataIndex: 'amount',
        customRender: ({ record }) => number_format(record.amount) + ' ₫',
    },
    {
        title: 'Đã thu',
        key: 'total_received',
        // dataIndex: 'transactions',
        customRender: ({ record }) => {
            let total = 0;

            record.transactions?.forEach(value => {
                total += value.amount;
            });

            return number_format(total) + ' ₫';
        },
    },
];

export default {
    props: {
        onFinishSelect: Function,
    },

    components: {
        UserEdit: defineAsyncComponent(() => import('~/dashboard/pages/users/Edit.vue')),
        ProductEdit: defineAsyncComponent(() => import('~/dashboard/pages/products/Edit.vue')),
        StockEdit: defineAsyncComponent(() => import('~/dashboard/pages/stocks/Edit.vue')),
        OrderEdit: defineAsyncComponent(() => import('~/dashboard/pages/orders/Edit.vue')),

        SearchOutlined, DownloadOutlined, PlusOutlined,
        ReloadOutlined, ShoppingCartOutlined, EditOutlined,
    },
    data() {
        return {
            userEditPageVisible: false,
            currentUserId: undefined,

            stockEditPageVisible: false,
            currentStockId: undefined,

            productEditPageVisible: false,
            currentProductId: undefined,

            orderEditPageVisible: false,
            currentOrderId: undefined,

            orders: [],
            ordersTableLoading: false,
            ordersTableColumns,
            ordersTablePagination: {
                position: 'both',
            },
            ordersTableSorts: undefined,
            ordersTableFilters: {},

            orderProductTableColumns,
            orderProductStockTableColumns,
            transactionsTableColumns,

            OrderStatus,
            configOrderStatus,
            OrderProductStockStatus,
        }
    },
    mounted() {
        if (!this.isModalMode) {
            this.ordersTablePagination.current = this.$route.query.page;

            this.ordersTableFilters = this.$route.query;
            delete this.ordersTableFilters.sort_by;
            delete this.ordersTableFilters.page;

            this.ordersTableSorts = this.$route.query.sort_by;
        }

        this.loadOrders();
    },
    computed: {
        /**
         * Is on modal / import mode
         * @return bool
         */
        isModalMode() {
            return this.onFinishSelect !== undefined;
        },

        ordersTableData() {
            return this.orders;
        },
    },
    methods: {
        number_format,
        date_format,
        moment,

        loadOrders(){
            this.ordersTableLoading = true;

            // Reset popup data
            this.currentUserId = undefined;
            this.currentStockId = undefined;
            this.currentProductId = undefined;
            this.currentOrderId = undefined;

            const params = {
                ...this.ordersTableFilters,
                sort_by: this.ordersTableSorts,
                page: this.ordersTablePagination.current,
            };

            if (!this.isModalMode) {
                this.$router.replace({
                    query: params,
                });
            }

            RequestRepository.get('/orders', { params })
                .then(res => {
                    const resData = res.data;

                    this.orders = resData.data || [];

                    this.ordersTablePagination = {
                        ...this.ordersTablePagination,
                        total: resData.total,
                        current: resData.current_page,
                        pageSize: resData.per_page,
                    };

                    // const newUrl = new URL(window.location.href);
                    // console.log(newUrl);
                    // newUrl.searchParams.append('page', resData.current_page);
                    // newUrl.searchParams.push('page', resData.current_page);
                    // this.$router.push(newUrl.pathname.replace(/\/dashboard/, '') + newUrl.search);
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.ordersTableLoading = false;
                });
        },

        calcCost(record) {
            return number_format((() => {
                let cost = 0;

                record.order_products.forEach(op_elm => {
                    op_elm.order_product_stocks.forEach(ops_elm => {
                        // stock.cost_amount
                        if (ops_elm.status === OrderProductStockStatus.STS_SOLD) {
                            cost += ops_elm.stock.cost_price;
                        }
                    });
                });

                return cost;
            })());
        },
        calcAmount(record) {
            return number_format((() => {
                let amount = 0;
                record.order_products.forEach(op_elm => {
                    op_elm.order_product_stocks.forEach(ops_elm => {
                        ops_elm.transactions.forEach(tnx_eml => {
                            amount += tnx_eml.amount;
                        });
                    });
                });

                return amount;
            })());
        },
        calcSell(record) {
            return number_format((() => {
                let sell = 0;

                record.order_products.forEach(op_elm => {
                    op_elm.order_product_stocks.forEach(ops_elm => {
                        // stock.cost_amount
                        if (ops_elm.status === OrderProductStockStatus.STS_SOLD) {
                            sell += ops_elm.amount;
                        }
                    });
                });

                return sell;
            })());
        },
        calcAdd(record) {
            return number_format((() => {
                let amount = 0;
                // addon transactions
                record.transactions.forEach(r_tnx_elm => {
                    amount += r_tnx_elm.amount;
                });

                return amount;
            })());
        },

        download() {
            const filters = this.ordersTableFilters;
            const downloadUrl = new URL(window.location.href);
            downloadUrl.pathname = RequestRepository.defaults.baseURL + '/orders';
            downloadUrl.searchParams.append('download', 'csv');
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
}
</script>
