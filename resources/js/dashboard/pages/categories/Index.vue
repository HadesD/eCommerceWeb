<template>
  <div>
    <a-button type="primary">
      Thêm mới
    </a-button>
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
  data() {
    return {
      categories: [],
    };
  },
  computed: {
    categoriesTree: () => {
      let tree = [];

      return tree;
    },
  },
  mounted(){
    axios.get('/api/categories')
      .then(res => {
        this.categories = res.data.data || [];
      });
  },
  methods: {
    onDragEnter(info) {
    },
    onDrop(info) {
    },
  },
};
</script>
