<template>
  <a-modal
    title="Thêm chuyên mục mới"
    :visible="visible"
    :confirm-loading="confirmLoading"
    okText="Tạo"
    @ok="ok"
    @cancel="cancel"
    >
    <a-form-model
      ref="ruleForm"
      :model="formData"
      :rules="rules"
      >
      <a-form-model-item label="Tên chuyên mục" ref="name" prop="name">
        <a-input
          v-model="formData.name"
          @change="onNameChanged"
          @blur="() => {$refs.name.onFieldBlur();$refs.slug.onFieldBlur()}"
          />
      </a-form-model-item>
      <a-form-model-item label="Đường dẫn URL (Slug)" ref="slug" prop="slug">
        <a-input
          v-model="formData.slug"
          @blur="() => $refs.slug.onFieldBlur"
          />
      </a-form-model-item>
      <a-form-model-item label="Mô tả ngắn">
        <a-textarea
          v-model="formData.description"
          placeholder="Nhập mô tả"
          :auto-size="{ minRows: 3, maxRows: 5  }"
          />
      </a-form-model-item>
      <a-form-model-item label="Chuyên mục cha">
        <a-form-model-item
          :style="{ display: 'inline-block', width: 'calc(100% - 40px)' }"
          >
          <a-spin :spinning="categoriesTreeLoading">
            <a-tree-select
              show-search
              allow-clear
              v-model="formData.parent_id"
              tree-data-simple-mode
              style="width: 100%"
              :dropdown-style="{ maxHeight: '400px', overflow: 'auto' }"
              :tree-data="categoriesTreeData"
              placeholder="Chuyên mục cha"
              :replaceFields="{ pId:'parent_id',title:'name',value:'id' }"
              />
          </a-spin>
        </a-form-model-item>
        <a-form-model-item
          :style="{ display: 'inline-block', float:'right' }"
          >
          <a-button type="primary" icon="reload" @click="reloadCategoriesTree" :loading="categoriesTreeLoading" />
        </a-form-model-item>
      </a-form-model-item>
    </a-form-model>
  </a-modal>
</template>
<script>
import {vietnameseNormalize} from '../../stringNormalize.js'

export default {
  props: {
    visible: Boolean,
    categories: Array,
    categoriesTreeLoading: Boolean,
  },
  data() {
    return {
      confirmLoading: false,
      formData: {
        name: '',
        parent_id: null,
        slug: '',
        description: '',
      },
      rules: {
        name: [
          { required: true, trigger: 'blur' },
        ],
        slug: [
          { required: true, trigger: 'blur' },
        ],
      },
    };
  },
  computed:{
    categoriesTreeData(){
      let data = this.categories;

      for (let i = 0; i < data.length; i++)
      {
        data[i].pId = data[i].parent_id;
      }

      return data;
    },
  },
  mounted(){
    // console.log(this.categories);
    // this.categoriesTreeData = this.categories;
  },
  methods: {
    onNameChanged(e){
      this.formData.slug = vietnameseNormalize(e.target.value);
    },
    reloadCategoriesTree(){
      this.$emit('updateCategories');
    },
    ok(e) {
      this.$refs.ruleForm.validate(valid => {
        if (!valid)
        {
          return false;
        }

        this.confirmLoading = true;

        axios
          .post('/api/categories', this.formData)
          .then(res => {
            this.$emit('handleOk');

            this.$message.success(`Tạo chuyên mục [${res.data.data.name}] thành công`);

            this.formData.name = '';
            this.formData.slug = '';
            this.formData.description = '';

            if (this.formData.parent_id)
            {
              this.reloadCategoriesTree();
            }
            else
            {
              this.$emit('updateCategories', res.data.data || []);
            }
          })
          .catch(err => {
            console.log(err);

            this.$message.error('Tạo chuyên mục thất bại');
          })
          .then(()=>{
            this.confirmLoading = false;
          });
      });
    },
    cancel(e) {
      this.$emit('handleCancel');
    },
  },
};
</script>
