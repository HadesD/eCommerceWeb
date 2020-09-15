<template>
  <div>
    <h2>
      Đơn đặt hàng
      <a-tooltip title="Làm mới">
        <a-button type="primary" icon="reload" :loading="ordersTableLoading" @click="() => {loadOrders(ordersTablePagination.current)}" />
      </a-tooltip>
      <router-link to="/orders/new">
        <a-tooltip title="Nhập kho">
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
        {{ configOrderStatus[record.status] }}
      </span>
      <span slot="cost_price" slot-scope="record" style="display:block;text-align:right;">
        {{ new Intl.NumberFormat().format(record.cost_price) }}
      </span>
      <span slot="time" slot-scope="record">
        Ngày tạo: {{ record.created_at }}<br />
        Ngày update: {{ record.updated_at }}
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
    title: 'Tên',
    dataIndex: 'name',
    key: 'name',
  },
  {
    title: 'Id/Imei',
    dataIndex: 'idi',
    key: 'idi',
  },
  {
    title: 'Trạng thái',
    key: 'status',
    scopedSlots: { customRender: 'status' },
  },
  {
    title: 'Giá lúc nhập (VND)',
    key: 'cost_price',
    scopedSlots: { customRender: 'cost_price' },
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
    this.loadOrders(this.ordersTablePagination.current);
  },
  computed: {
    ordersTableData(){
      return this.orders;
    },
    configOrderStatus(){
      return {
        0: 'Có sẵn',
        1: 'Đã bán',
        2: 'Hỏng / Lỗi',
      }
    },
  },
  methods: {
    loadOrders(page){
      this.ordersTableLoading = true;
      axios.get('/api/orders')
        .then(res => {
          this.orders = res.data.data || [];
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
