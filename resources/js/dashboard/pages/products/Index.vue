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
    <a-col :span="4" style="border-right:1px solid #CCC;">
      <h2>
        Chuyên mục
        <a-button type="primary" icon="plus" @click="showAddCategoryModal" style="float:right;" />
      </h2>
      <a-spin :spinning="categoriesTreeLoading">
        <a-tree
          default-expand-all show-line
          @select="onCategoriesTreeSelect" @expand="onCategoriesTreeExpand"
          :tree-data="categoriesTreeData"
          >
        </a-tree>
      </a-spin>
    </a-col>
    <a-col :span="20">
      <h2>
        Sản phẩm
        <router-link to="/products/new">
          <a-button type="primary" icon="plus" style="float:right;" />
        </router-link>
      </h2>
      <a-table
        :columns="productsTableColumns"
        :data-source="productsTableData"
        :loading="productsTableLoading"
        :row-key="record => record.id"
        :pagination="productsTablePagination"
        @change="onProductsTablePaginationChanged"
        >
        <span slot="customTitle"><a-icon type="smile-o" /> Name</span>
        <span slot="time" slot-scope="record">
            Ngày tạo: {{ record.created_at }}<br />
            Ngày update: {{ record.updated_at }}
        </span>
        <span slot="action" slot-scope="record">
          <router-link :to="`/products/${record.id}/edit`">Sửa</router-link>
          <a-divider type="vertical"></a-divider>
          <a>Delete</a>
        </span>
      </a-table>
    </a-col>
  </a-row>
</template>

<script>
const productsTableColumns = [
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
    title: 'Trạng thái',
    dataIndex: 'status',
    key: 'status',
  },
  {
    title: 'Giá',
    dataIndex: 'price',
    key: 'price',
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

export default {
  components: {
    AddCategoryModal: () => import('../../components/AddCategoryModal.vue'),
  },
  data() {
    return {
      categories: [],
      addCategoryModalVisible: false,
      categoriesTreeLoading: false,

      products: [],
      productsTableLoading: false,
      productsTableColumns,
      productsTablePagination: {
        position: 'both',
      },
    };
  },
  mounted(){
    this.loadCategoriesTree();

    this.loadProducts(0, 0);
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
      }

      return data;
    },
    productsTableData(){
      return this.products;
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
      this.loadProducts(keys, 1);
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

    // Product
    loadProducts(category_id, page){
      this.productsTableLoading = true;

      axios.get(`/api/products?category_id=${category_id}&page=${page}`)
        .then(res => {
          const resData = res.data;
          this.products = resData.data || [];

          const newPagi = {
            total: resData.total,
            current: resData.current_page,
            pageSize: resData.per_page,
          };
          this.productsTablePagination = {...newPagi};
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
          this.productsTableLoading = false;
        });
    },
    onProductsTablePaginationChanged(pagination){
      console.log(pagination);
      this.loadProducts(0, pagination.current);
    },
  },
}
</script>
