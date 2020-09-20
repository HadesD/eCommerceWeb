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
      <h2>
        Chuyên mục
        <a-tooltip title="Thêm chuyên mục">
          <a-button type="primary" icon="plus" @click="showAddCategoryModal" style="float:right;" />
        </a-tooltip>
      </h2>
      <a-spin :spinning="categoriesTreeLoading">
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
    </a-col>
  </a-row>
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
    }
  },
  mounted() {
    this.loadCategoriesTree();

    this.loadStocks(this.currentCategoryId, 0);
  },
  computed: {
    categoriesTreeData(){
      const getParent = (key, tree) => {
        let parent;
        for (let i = 0; i < tree.length; i++)
        {
          const node = tree[i];
          if (node.key === key)
          {
            parent = node;
          }
          else if (node.children.length)
          {
            parent = getParent(key, node.children);
          }
        }

        return parent;
      };

      const sortedCategories = this.categories.sort((a, b) => a.parent_id - b.parent_id);

      let data = [];
      for (let i = 0; i < sortedCategories.length; i++)
      {
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
      return {
        0: 'Có sẵn',
        1: 'Đã bán',
        2: 'Hỏng / Lỗi',
      }
    },
  },
  methods: {
    // CategoriesTree
    loadCategoriesTree(){
      this.categoriesTreeLoading = true;
      axios.get('/api/categories')
        .then(res => {
          this.categories = res.data.data || [];
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
          this.categoriesTreeLoading = false;
        });
    },
    updateCategories(cats) {
      this.loadCategoriesTree();
    },
    onCategoriesTreeSelect(keys, event) {
      this.currentCategoryId = keys;

      this.loadStocks(this.currentCategoryId, 1);
    },
    onCategoriesTreeExpand() {
      console.log('Trigger Expand');
    },

    // Modal
    showAddCategoryModal() {
      this.addCategoryModalVisible = true;
    },
    addCategoryModalHandleOk(e){
      // this.addCategoryModalVisible = false;
    },
    addCategoryModalHandleCancel(e){
      this.addCategoryModalVisible = false;
    },

    loadStocks(category_id, page){
      this.stocksTableLoading = true;
      axios.get(`/api/stocks?page=${page}&category_id=${category_id}`)
        .then(res => {
          const resData = res.data;
          this.stocks = resData.data || [];

          const newPagi = {
            total: resData.total,
            current: resData.current_page,
            pageSize: resData.per_page,
          };
          this.stocksTablePagination = {...newPagi};
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
      this.loadStocks(this.currentCategoryId, pagination.current);
    },

    onDeleteConfirmed(record){
      axios.delete(`/api/stocks/${record.id}`)
        .then(res => {
          this.$message.success('Xóa thành công');

          this.loadStocks(this.currentCategoryId, this.stocksTablePagination.current);
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
