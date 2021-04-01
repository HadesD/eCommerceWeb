<template>
    <div>
        <h2>
            <span>Hoá Đơn</span>
            <a-tooltip title="Làm mới">
                <a-button type="primary" icon="reload" :loading="ordersTableLoading" @click="() => loadOrders(ordersTablePagination.current)" />
            </a-tooltip>
            <router-link to="/orders/new">
                <a-tooltip title="Thêm đơn">
                <a-button type="primary" icon="plus" style="float:right;" />
                </a-tooltip>
            </router-link>
        </h2>
        <a-table
            :columns="ordersTableColumns"
            :data-source="ordersTableData"
            :loading="ordersTableLoading"
            :row-key="record => record.id"
            :pagination="ordersTablePagination"
            @change="(pagination) => loadOrders(pagination.current)"
        >
            <template slot="customer" slot-scope="value, record">
                <div v-if="value && record.customer">
                    <div>
                        <span>#{{ value }}. {{ record.customer.name }}</span>
                        <a-button icon="search" @click="() => { currentCustomerId = value; userEditPageVisible = true; }" size="small" />
                    </div>
                    <div>Phone: {{ record.customer.phone || 'Chưa có' }}</div>
                </div>
            </template>
            <template slot="status" slot-scope="record">
                <a-tag v-if="configOrderStatus[record.status]" :color="configOrderStatus[record.status].color">{{ configOrderStatus[record.status].name }}</a-tag>
            </template>
            <template slot="total_amount" slot-scope="record">
                <span>{{ totalAmount(record) }}</span>
            </template>
            <template slot="order_product" slot-scope="record">
                <div v-for="(p) in record.order_products" :key="p.id">
                    <div>
                        <span>{{ p.product.name }} [Số lượng: {{ p.quantity }}]</span>
                        <a-button icon="search" @click="() => { currentProductId = p.product.id; productEditPageVisible = true; }" size="small" />
                    </div>
                    <ul>
                        <li v-for="(ps) in p.order_product_stocks" :key="ps.id">
                            <span>{{ ps.stock.name }} ({{ ps.stock.idi }})</span>
                            <a-button icon="search" @click="() => { currentStockId = ps.stock.id; stockEditPageVisible = true; }" size="small" />
                        </li>
                    </ul>
                </div>
                <div v-for="addon_tnx in record.transactions" :key="addon_tnx.id">
                    {{ addon_tnx.description }} ({{ number_format(addon_tnx.amount) }})
                </div>
            </template>
            <template slot="time" slot-scope="record">
                <div>Tạo: {{ date_format(record.created_at) }}</div>
                <div>Update: {{ date_format(record.updated_at) }}</div>
            </template>
            <template slot="action" slot-scope="record">
                <template v-if="!onFinishSelect">
                    <router-link :to="`/orders/${record.id}/edit`">
                        <a-button type="primary" icon="edit" />
                    </router-link>
                    <a-popconfirm title="Chắc chưa?" @confirm="() => onDeleteConfirmed(record)">
                        <a-icon slot="icon" type="question-circle-o" style="color: red" />
                        <a-button type="danger" icon="delete" />
                    </a-popconfirm>
                </template>
                <template v-else>
                    <a-button type="primary" icon="shopping-cart" @click="() => onFinishSelect(record)">Chọn</a-button>
                </template>
            </template>
        </a-table>

        <a-modal
            :visible="userEditPageVisible"
            @cancel="() => userEditPageVisible = false"
            :footer="false"
            :width="800"
        >
            <UserEdit :userId="currentCustomerId" />
        </a-modal>

        <a-modal
            :visible="stockEditPageVisible"
            @cancel="() => stockEditPageVisible = false"
            :footer="false"
            width="95vw"
        >
            <StockEdit :stockId="currentStockId" />
        </a-modal>

        <a-modal
            :visible="productEditPageVisible"
            @cancel="() => productEditPageVisible = false"
            :footer="false"
            width="95vw"
        >
            <ProductEdit :productId="currentProductId" />
        </a-modal>
    </div>
</template>

<script>
import OrderStatus, { Config as configOrderStatus } from '../../configs/OrderStatus';
import { number_format, date_format } from '../../../helpers';

const ordersTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
        key: 'id',
    },
    {
        title: 'Trạng thái',
        key: 'status',
        scopedSlots: { customRender: 'status' },
    },
    {
        title: 'Ghi chú',
        dataIndex: 'note',
    },
    {
        title: 'Tổng Thu/Giá Nhập',
        key: 'total_amount',
        scopedSlots: { customRender: 'total_amount' },
    },
    {
        title: 'Đặt hàng',
        key: 'order_product',
        scopedSlots: { customRender: 'order_product' },
    },
    {
        title: 'Khách hàng',
        dataIndex: 'customer_id',
        scopedSlots: {
            customRender: 'customer',
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
        fixed: 'right',
    },
];

export default {
    props: {
        onFinishSelect: Function,
    },
    components: {
        UserEdit: () => import('../users/Edit'),
        StockEdit: () => import('../products/Edit'),
        ProductEdit: () => import('../stocks/Edit'),
    },
    data() {
        return {
            userEditPageVisible: false,
            currentCustomerId: undefined,

            stockEditPageVisible: false,
            currentStockId: undefined,

            productEditPageVisible: false,
            currentProductId: undefined,

            orders: [],
            ordersTableLoading: false,
            ordersTableColumns,
            ordersTablePagination: {
                position: 'both',
            },
        }
    },
    mounted() {
        this.ordersTablePagination.current = (parseInt(this.$route.query.page) || 1)

        this.loadOrders(this.ordersTablePagination.current);
    },
    computed: {
        ordersTableData(){
            return this.orders;
        },

        OrderStatus() {
            return OrderStatus;
        },
        configOrderStatus() {
            return configOrderStatus;
        },
    },
    methods: {
        number_format,
        date_format,

        loadOrders(page){
        this.ordersTableLoading = true;
        axios.get('/api/orders', {
            params: {
                page,
            }
        })
            .then(res => {
            const resData = res.data;
            this.orders = resData.data || [];

            const newPagi = {
                total: resData.total,
                current: resData.current_page,
                pageSize: resData.per_page,
            };
            this.ordersTablePagination = {...newPagi};

            //   if (this.$route.query.page != resData.current_page) {
                // this.$router.push('/orders/index?page='+resData.current_page);
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
            this.ordersTableLoading = false;
            });
        },

        totalAmount(record) {
            let amount = 0;
            let cost = 0;
            // addon transactions
            record.transactions.forEach(r_tnx_elm => {
                amount += r_tnx_elm.amount;
            });

            record.order_products.forEach(op_elm => {
                op_elm.order_product_stocks.forEach(ops_elm => {
                    ops_elm.transactions.forEach(tnx_eml => {
                        amount += tnx_eml.amount;
                    });

                    // stock.cost_amount
                    cost += ops_elm.stock.cost_price;
                });
            });

            return `${number_format(amount)} / ${number_format(cost)}`;
        },

        onDeleteConfirmed(record){
        return axios.delete(`/api/orders/${record.id}`)
            .then(res => {
            this.$message.success('Xóa thành công');

            this.loadOrders(this.ordersTablePagination.current);
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
