<template>
  <div>
    <a-button type="primary" @click="showModal">
      Thêm mới
    </a-button>
    <AddCategoryModal
      :visible="modalVisible"
      @handleOk="addCategoryModalHandleOk"
      @handleCancel="addCategoryModalHandleCancel"
      />
    <a-tree
      :show-line="true"
      class="draggable-tree"
      draggable
      :tree-data="categoriesTree"
      @dragenter="onDragEnter"
      @drop="onDrop"
      />
  </div>
</template>

<script>
import axios from 'axios'

export default {
  components: {
    AddCategoryModal: () => import('../../components/AddCategoryModal.vue'),
  },
  data() {
    return {
      categories: [],
      modalVisible: false,
    };
  },
  computed: {
    categoriesTree: () => {
      let tree = [];

      return tree;
    },
  },
  mounted(){
    console.log(this.$dialog);
    axios.get('/api/categories')
      .then(res => {
        this.categories = res.data.data || [];
      });
  },
  methods: {
    showModal() {
      this.modalVisible = true;
    },
    addCategoryModalHandleOk(e){
      // this.modalVisible = false;
    },
    addCategoryModalHandleCancel(e){
      this.modalVisible = false;
    },
    onDragEnter(info) {
    },
    onDrop(info) {
    },
  },
};
</script>
