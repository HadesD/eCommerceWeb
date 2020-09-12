<template>
  <a-row :gutter="16">
    <a-col :span="4" style="border-right:1px solid #CCC;">
      <h2>
        Chuyên mục
        <a-button type="primary" icon="plus" @click="showAddCategoryModal" style="float:right;" />
      </h2>
      <AddCategoryModal
        :visible="modalVisible"
        :categories="categories"
        :categoriesTreeLoading="categoriesTreeLoading"
        @handleOk="addCategoryModalHandleOk"
        @handleCancel="addCategoryModalHandleCancel"
        @updateCategories="updateCategories"
        />
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
    </a-col>
  </a-row>
</template>

<script>
export default {
  components: {
    AddCategoryModal: () => import('../../components/AddCategoryModal.vue'),
  },
  data() {
    return {
      categories: [],
      modalVisible: false,
      categoriesTreeLoading: false,
    };
  },
  mounted(){
    this.loadCategoriesTree();
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
  },
  methods: {
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
      console.log('Trigger Select', keys, event);
    },
    onCategoriesTreeExpand() {
      console.log('Trigger Expand');
    },
    showAddCategoryModal() {
      this.modalVisible = true;
    },
    addCategoryModalHandleOk(e){
      // this.modalVisible = false;
    },
    addCategoryModalHandleCancel(e){
      this.modalVisible = false;
    },
  },
}
</script>
