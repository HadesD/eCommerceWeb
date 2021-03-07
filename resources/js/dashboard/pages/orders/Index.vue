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
        <a-tag :color="configOrderStatus[record.status].color">{{ configOrderStatus[record.status].title }}</a-tag>
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
      </template>
      <span slot="transaction_num" slot-scope="record">
        {{ record.transactions.length }}
      </span>
      <span slot="customer" slot-scope="record">
        {{ record.customer ? record.customer.name : record.customer_id }}
      </span>
      <span slot="time" slot-scope="record">
        Tạo: {{ record.created_at }}<br />
        Update: {{ record.updated_at }}
      </span>
      <span slot="action" slot-scope="record">
        <router-link :to="`/orders/${record.id}/edit`">
          <a-icon type="edit" /> Sửa
        </router-link>
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
    title: 'Đặt hàng',
    key: 'order_product',
    scopedSlots: { customRender: 'order_product' },
  },
  {
    title: 'Giao dịch thêm',
    key: 'transaction_num',
    scopedSlots: { customRender: 'transaction_num' },
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
    configOrderStatus(){
      return {
        0: {
          title: 'Đang xử lí',
          color: '#F50',
        },
        10: {
          title: 'Đã hủy bỏ',
          color: 'darkgrey',
        },
        50: {
          title: 'Đã thanh toán',
          color: 'green',
        },
        51: {
          title: 'Đang thanh toán',
          color: 'red',
        },
        100: {
          title: 'Đang ship',
          color: 'orange',
        },
        200: {
          title: 'Hoàn tất',
          color: 'blue',
        },
      }
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

          if (this.$route.query.page != resData.current_page)
          {
            this.$router.push('/orders/index?page='+resData.current_page);
          }
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
          this.ordersTableLoading = false;
        });
    },

    onOrdersTablePaginationChanged(pagination){
      this.loadOrders(pagination.current);
    },

    onDeleteConfirmed(record){
      axios.delete(`/api/orders/${record.id}`)
        .then(res => {
          this.$message.success('Xóa thành công');

          this.loadOrders(this.ordersTablePagination.current);
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Xóa thất bại');
        })
        .then(()=>{
        });
    },
  },
}
</script>
