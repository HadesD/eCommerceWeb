<template>
    <div>
        <a-page-header title="Hoá Đơn">
            <template slot="tags">
                <a-tooltip title="Làm mới">
                    <a-button type="primary" icon="reload" :loading="ordersTableLoading" @click="() => loadOrders({})" />
                </a-tooltip>
            </template>
            <template slot="extra">
                <a-tooltip title="Tải CSV">
                    <a-button type="primary" icon="download" :disabled="orders.length <= 0" @click="() => download()" />
                </a-tooltip>
                <a-tooltip title="Thêm đơn">
                    <a-button type="primary" icon="plus" @click="() => { currentOrderId = undefined; orderEditPageVisible = true; }" />
                </a-tooltip>
            </template>
        </a-page-header>
        <a-table
            defaultExpandAllRows
            :scroll="($screen.xs || $screen.sm || $screen.md) ? { x: 1300, y: '85vh' } : {}"
            :size="($screen.xs || $screen.sm || $screen.md) ? 'small' : 'default'"
            :columns="ordersTableColumns"
            :data-source="ordersTableData"
            :loading="ordersTableLoading"
            :row-key="record => record.id"
            :pagination="ordersTablePagination"
            @change="(pagination, filters, sorter) => {
                ordersTableFilters = filters;
                ordersTableSorts = (sorter.column && sorter.columnKey) ? ((sorter.order === 'descend' ? '-' : '+') + sorter.columnKey) : undefined;
                loadOrders({page: pagination.current});
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

            <template slot="customer" slot-scope="value, record">
                <div v-if="value && record.customer">
                    <div>
                        <span>#{{ value }}. {{ record.customer.name }}</span>
                        <a-button icon="search" @click="() => { currentUserId = value; userEditPageVisible = true; }" size="small" />
                    </div>
                    <div>Phone: {{ record.customer.phone || 'Chưa có' }}</div>
                </div>
            </template>
            <template slot="status" slot-scope="record">
                <a-tag v-if="configOrderStatus[record.status]" :color="configOrderStatus[record.status].color">{{ configOrderStatus[record.status].name }}</a-tag>
            </template>
            <template slot="total_amount" slot-scope="record">
                <div>
                    <span>Nhập: </span>
                    <span>{{ number_format((() => {
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
                    })()) }} ₫</span>
                </div>
                <div>
                    <span>Bán: </span>
                    <span>{{ number_format((() => {
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
                    })()) }} ₫</span>
                </div>
                <div>
                    <span>Thu Kho: </span>
                    <span>{{ number_format((() => {
                        let amount = 0;
                        record.order_products.forEach(op_elm => {
                            op_elm.order_product_stocks.forEach(ops_elm => {
                                ops_elm.transactions.forEach(tnx_eml => {
                                    amount += tnx_eml.amount;
                                });
                            });
                        });

                        return amount;
                    })()) }} ₫</span>
                </div>
                <div>
                    <span>GD Thêm: </span>
                    <span>{{ number_format((() => {
                        let amount = 0;
                        // addon transactions
                        record.transactions.forEach(r_tnx_elm => {
                            amount += r_tnx_elm.amount;
                        });

                        return amount;
                    })()) }} ₫</span>
                </div>
            </template>
            <template slot="time" slot-scope="record">
                <div>Tạo: {{ date_format(record.created_at) }}</div>
                <div>Update: {{ date_format(record.updated_at) }}</div>
                <div v-if="record.updated_user">
                    <span>Cuối bởi: {{ record.updated_user.name }}</span>
                    <a-button icon="search" @click="() => { currentUserId = record.updated_user_id; userEditPageVisible = true; }" size="small" />
                </div>
            </template>
            <template slot="action" slot-scope="record">
                <template v-if="!onFinishSelect">
                    <a-button type="primary" icon="edit" @click="() => { currentOrderId = record.id; orderEditPageVisible = true; }" />
                </template>
                <template v-else>
                    <a-button type="primary" icon="shopping-cart" @click="() => onFinishSelect(record)">Chọn</a-button>
                </template>
            </template>

            <template slot="expandedRowRender" slot-scope="o">
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
                    <template slot="expandedRowRender" slot-scope="op">
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
            <OrderEdit :orderId="currentOrderId" @orderUpdated="(orderId) => loadOrders({})" />
        </a-modal>
    </div>
</template>

<script>
import OrderStatus, { Config as configOrderStatus } from '../../configs/OrderStatus';
import OrderProductStockStatus from '../../configs/OrderProductStockStatus';
import { number_format, date_format } from '../../../helpers';
import moment from 'moment';

const ordersTableColumns = [
    {
        title: '#ID',
        dataIndex: 'id',
        scopedSlots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Trạng thái',
        key: 'status',
        scopedSlots: { customRender: 'status' },
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
        customRender: (text) => date_format(text),
        scopedSlots: {
            filterDropdown: 'filterRangeDate',
        },
        sorter: true,
    },
    {
        title: 'Khách hàng',
        dataIndex: 'customer_id',
        scopedSlots: {
            customRender: 'customer',
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Tổng Tiền',
        key: 'total_amount',
        scopedSlots: { customRender: 'total_amount' },
    },
    {
        title: 'Ghi chú',
        dataIndex: 'note',
        scopedSlots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
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
        dataIndex: 'product',
        customRender: (product) => product.name,
    },
];

const orderProductStockTableColumns = [
    {
        title: 'Tên hàng xuất kho',
        key: 'name',
        dataIndex: 'stock',
        customRender: (stock) => stock.name,
    },
    {
        title: 'Idi/Imei',
        key: 'idi',
        dataIndex: 'stock',
        customRender: (stock) => stock.idi,
    },
    {
        title: 'Giá nhập',
        key: 'cost_price',
        dataIndex: 'stock',
        customRender: (stock) => number_format(stock.cost_price),
    },
    {
        title: 'Giá bán',
        dataIndex: 'amount',
        customRender: (value) => number_format(value),
    },
    {
        title: 'Đã thu',
        key: 'total_received',
        dataIndex: 'transactions',
        customRender: (transactions) => {
            let total = 0;

            transactions.forEach(value => {
                total += value.amount;
            });

            return number_format(total);
        },
    },
];

export default {
    props: {
        onFinishSelect: Function,
    },
    components: {
        UserEdit: () => import('../users/Edit'),
        StockEdit: () => import('../stocks/Edit'),
        ProductEdit: () => import('../products/Edit'),
        OrderEdit: () => import('../orders/Edit'),
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
        this.ordersTablePagination.current = (parseInt(this.$route.query.page) || 1)

        this.loadOrders({});
    },
    computed: {
        ordersTableData() {
            return this.orders;
        },
    },
    methods: {
        number_format,
        date_format,
        moment,

        loadOrders({page}){
            this.ordersTableLoading = true;

            // Reset popup data
            this.currentUserId = undefined;
            this.currentStockId = undefined;
            this.currentProductId = undefined;
            this.currentOrderId = undefined;

            axios.get('/api/orders', {
                params: {
                    page: page || this.ordersTablePagination.current,
                    ...this.ordersTableFilters,
                    sort_by: this.ordersTableSorts,
                }
            })
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
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.ordersTableLoading = false;
                });
        },

        download() {
            const filters = this.ordersTableFilters;
            const downloadUrl = new URL(window.location.href);
            downloadUrl.pathname = '/api/orders';
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
