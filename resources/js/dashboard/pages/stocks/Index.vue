<template>
  <div>
    <h2>
      Kho hàng
      <a-tooltip title="Làm mới">
        <a-button type="primary" icon="reload" :loading="stocksTableLoading" @click="() => {loadStocks(stocksTablePagination.current)}" />
      </a-tooltip>
      <router-link to="/stocks/new">
        <a-tooltip title="Nhập kho">
          <a-button type="primary" icon="plus" style="float:right;" />
        </a-tooltip>
      </router-link>
    </h2>
    <a-table
      :columns="stocksTableColumns"
      :data-source="stocksTableData"
      :loading="stocksTableLoading"
      :row-key="record => record.id"
      :pagination="stocksTablePagination"
      @change="onStocksTablePaginationChanged"
      >
      <span slot="status" slot-scope="record">
        {{ configStockStatus[record.status] }}
      </span>
      <span slot="cost_price" slot-scope="record" style="display:block;text-align:right;">
        {{ new Intl.NumberFormat().format(record.cost_price) }}
      </span>
      <span slot="time" slot-scope="record">
        Ngày tạo: {{ record.created_at }}<br />
        Ngày update: {{ record.updated_at }}
      </span>
      <span slot="action" slot-scope="record">
        <router-link :to="`/stocks/${record.id}/edit`">
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
const stocksTableColumns = [
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
    title: 'Ngày nhập',
    dataIndex: 'in_date',
    key: 'in_date',
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
      stocks: [],
      stocksTableLoading: false,
      stocksTableColumns,
      stocksTablePagination: {
        position: 'both',
      },
    }
  },
  mounted() {
    this.loadStocks(0);
  },
  computed: {
    stocksTableData(){
      return this.stocks;
    },
    configStockStatus(){
      return {
        0: 'Có sẵn',
        1: 'Đã bán',
        2: 'Hỏng / Lỗi',
      }
    },
  },
  methods: {
    loadStocks(page){
      this.stocksTableLoading = true;
      axios.get(`/api/stocks?page=${page}`)
        .then(res => {
          this.stocks = res.data.data || [];
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
          this.stocksTableLoading = false;
        });
    },

    onStocksTablePaginationChanged(pagination){
      this.loadStocks(pagination.current);
    },

    onDeleteConfirmed(record){
      axios.delete(`/api/stocks/${record.id}`)
        .then(res => {
          this.$message.success('Xóa thành công');

          this.loadStocks(this.stocksTablePagination.current);
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
