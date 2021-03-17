<template>
  <div>
    <h2>
      Đơn đặt hàng
      <a-tooltip title="Làm mới">
        <a-button type="primary" icon="reload" :loading="ordersTableLoading" @click="() => {loadOrders(ordersTablePagination.current)}" />
      </a-tooltip>
      <router-link to="/orders/new">
        <a-tooltip title="Thêm đơn hàng">
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
      @change="onOrdersTablePaginationChanged"
      >
      <span slot="status" slot-scope="record">
        <a-tag :color="configOrderStatus[record.status].color">{{ configOrderStatus[record.status].name }}</a-tag>
      </span>
      <span slot="total_amount" slot-scope="record">
        <span>{{ totalAmount(record) }}</span>
      </span>
      <template slot="order_product" slot-scope="record">
        <div v-for="(p) in record.order_products" :key="p.id">
          <a-tooltip title="Xem" v-if="p.product ? true : false">
            <RouterLink :to="'/products/'+p.product.id+'/edit'">{{ p.product.name }} [Số lượng: {{ p.quantity }}]</RouterLink>
          </a-tooltip>
          <ul>
            <li v-for="(ps) in p.order_product_stocks" :key="ps.id">
              <a-tooltip title="Xem" v-if="ps.stock ? true : false">
                <RouterLink :to="'/stocks/'+ps.stock.id+'/edit'">{{ ps.stock.name }} ({{ ps.stock.idi }})</RouterLink>
              </a-tooltip>
            </li>
          </ul>
        </div>
        <div v-for="addon_tnx in record.transactions" :key="addon_tnx.id">
            {{ addon_tnx.description }} ({{ number_format(addon_tnx.amount) }})
        </div>
      </template>
      <span slot="customer" slot-scope="record">
        {{ record.customer ? record.customer.name : record.customer_id }}
      </span>
      <span slot="time" slot-scope="record">Tạo: {{ record.created_at }}<br />Update: {{ record.updated_at }}</span>
      <span slot="action" slot-scope="record">
        <router-link :to="`/orders/${record.id}/edit`"><a-icon type="edit" /> Sửa</router-link>
        <a-divider type="vertical"></a-divider>
        <a-popconfirm title="Chắc chưa?" @confirm="()=>{onDeleteConfirmed(record)}">
          <a-icon slot="icon" type="question-circle-o" style="color: red" />
            <a href="#"><a-icon type="delete" /> Xóa</a>
        </a-popconfirm>
      </span>
    </a-table>
  </div>
</template>

<script>
import OrderStatus from '../../configs/OrderStatus';
import { number_format } from '../../../helpers';

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
  /*{
    title: 'Khách hàng',
    key: 'customer',
    scopedSlots: { customRender: 'customer' },
  },*/
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
  data() {
    return {
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
    configOrderStatus() {
        return OrderStatus;
    },
    number_format() {
        return number_format;
    },
  },
  methods: {
    loadOrders(page){
      this.ordersTableLoading = true;
      axios.get('/api/orders?page='+page)
        .then(res => {
          const resData = res.data;
          this.orders = resData.data || [];

          const newPagi = {
            total: resData.total,
            current: resData.current_page,
            pageSize: resData.per_page,
          };
          this.ordersTablePagination = {...newPagi};

          if (this.$route.query.page != resData.current_page) {
            this.$router.push('/orders/index?page='+resData.current_page);
          }
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

    onOrdersTablePaginationChanged(pagination){
      this.loadOrders(pagination.current);
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
