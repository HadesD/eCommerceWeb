<template>
  <div>
    <AddCategoryModal
      :visible="addCategoryModalVisible"
      :categories="categories"
      :categoriesTreeLoading="categoriesTreeLoading"
      @handleOk="addCategoryModalHandleOk"
      @handleCancel="addCategoryModalHandleCancel"
      @updateCategories="updateCategories"
      />
    <a-spin :spinning="productInfoLoading">
      <h2>{{ $route.params.id ? `Sửa sản phẩm #${$route.params.id}` : 'Đăng bán sản phẩm mới' }}</h2>
      <a-form-model
        ref="ruleForm"
        :model="formData"
        :rules="rules"
        :label-col="labelCol"
        :wrapper-col="wrapperCol"
        >
        <a-form-model-item label="Tên sản phẩm" ref="name" prop="name">
          <a-input
            @change="onNameChanged"
            v-model="formData.name"
            @blur="() => {$refs.name.onFieldBlur();$refs.slug.onFieldBlur()}"
            />
        </a-form-model-item>
        <a-form-model-item label="Đường dẫn URL (Slug)" ref="slug" prop="slug">
          <a-input
            v-model="formData.slug"
            @blur="() => $refs.slug.onFieldBlur()"
            />
        </a-form-model-item>
        <a-form-model-item label="Giá bán" ref="price" prop="price">
          <a-input
            v-model="formData.price"
            @blur="() => $refs.price.onFieldBlur()"
            type="number"
            >
          </a-input>
          Xem trước: {{ new Intl.NumberFormat().format(formData.price) }} VND
        </a-form-model-item>
        <a-form-model-item label="Trạng thái" ref="status" prop="status">
          <a-select
            v-model="formData.status"
            @blur="() => $refs.status.onFieldBlur()"
            >
            <a-select-option :value="0">
              Bản nháp
            </a-select-option>
            <a-select-option :value="1">
              Đang bán
            </a-select-option>
            <a-select-option :value="2" :disabled="$route.params.id ? false : true">
              Hết hàng
            </a-select-option>
          </a-select>
        </a-form-model-item>
        <a-form-model-item label="Chuyên mục cha" ref="categories_id" prop="categories_id">
          <a-form-model-item
            style="display: inline-block; margin-right: 5px;"
            >
            <a-tooltip title="Thêm chuyên mục">
              <a-button type="primary" icon="plus" @click="showAddCategoryModal" />
            </a-tooltip>
          </a-form-model-item>
          <a-form-model-item
            :style="{ display: 'inline-block', width: 'calc(100% - 80px)' }"
            >
            <a-spin :spinning="categoriesTreeLoading">
              <a-tree-select
                show-search
                allow-clear
                multiple
                tree-data-simple-mode
                treeNodeFilterProp="title"
                v-model="formData.categories_id"
                style="width: 100%"
                :dropdown-style="{ maxHeight: '400px', overflow: 'auto' }"
                :tree-data="categoriesTreeData"
                placeholder="Chuyên mục"
                :replaceFields="{ pId:'parent_id',title:'name',value:'id' }"
                @blur="() => $refs.categories_id.onFieldBlur()"
                @change="() => $refs.categories_id.onFieldBlur()"
                />
            </a-spin>
          </a-form-model-item>
          <a-form-model-item
            style="display: inline-block; margin-left: 5px;"
            >
            <a-tooltip title="Làm mới">
              <a-button type="primary" icon="reload" @click="reloadCategoriesTree" :loading="categoriesTreeLoading" />
            </a-tooltip>
          </a-form-model-item>
        </a-form-model-item>
        <a-tabs default-active-key="description" @change="(k) => $refs[k] && $refs[k].focus()">
          <a-tab-pane key="description" tab="Mô tả ngắn">
            <a-form-model-item label="Mô tả ngắn">
              <a-textarea
                v-model="formData.description"
                ref="description"
                placeholder="Nhập mô tả"
                :auto-size="{ minRows: 3, maxRows: 10 }"
                />
            </a-form-model-item>
          </a-tab-pane>
          <a-tab-pane key="detail" tab="Chi tiết sản phẩm">
            <a-form-model-item label="Mô tả ngắn">
              <a-textarea
                v-model="formData.detail"
                ref="detail"
                placeholder="Nhập mô tả"
                :auto-size="{ minRows: 3, maxRows: 10 }"
                />
            </a-form-model-item>
          </a-tab-pane>
          <a-tab-pane key="specification" tab="Thông số sản phẩm">
            <a-form-model-item label="Mô tả ngắn">
              <a-textarea
                v-model="formData.specification"
                ref="specification"
                placeholder="Nhập mô tả"
                :auto-size="{ minRows: 3, maxRows: 10 }"
                />
            </a-form-model-item>
          </a-tab-pane>
        </a-tabs>
        <a-form-model-item :wrapper-col="{ span: 14, offset: 4 }">
          <a-button type="primary" @click="onSubmit">
            {{ $route.params.id ? 'Sửa' : 'Đăng bán' }}
          </a-button>
          <a-button style="margin-left: 10px;" @click="resetForm">Reset</a-button>
        </a-form-model-item>
      </a-form-model>
    </a-spin>
  </div>
</template>
<script>
import {vietnameseNormalize} from '../../../stringNormalize.js'

export default {
  components: {
    AddCategoryModal: () => import('../../components/AddCategoryModal.vue'),
  },
  data() {
    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },
      productInfoLoading: false,
      categoriesTreeLoading: false,
      addCategoryModalVisible: false,
      categories: [],

      formData: {
        id: undefined,
        name: '',
        slug: '',
        categories_id: [],
        description: '',
        detail: '',
        specification: '',
        price: undefined,
        status: 0,
      },
      rules: {
        name: [
          { required: true, trigger: 'blur' },
        ],
        slug: [
          { required: true, trigger: 'blur' },
        ],
        price: [
          { required: true, trigger: 'blur' },
        ],
        status: [
          { required: true, trigger: 'blur' },
        ],
        categories_id: [
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
    console.log(this.$route);
    this.formData.id = this.$route.params.id;

    if (this.formData.id)
    {
      this.loadProduct(this.formData.id)
    }

    this.reloadCategoriesTree();
  },
  methods: {
    loadCategoriesTree(){
      this.reloadCategoriesTree();
    },
    reloadCategoriesTree(){
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
    showAddCategoryModal() {
      this.addCategoryModalVisible = true;
    },
    addCategoryModalHandleOk(e){
      // this.addCategoryModalVisible = false;
    },
    addCategoryModalHandleCancel(e){
      this.addCategoryModalVisible = false;
    },

    loadProduct(id){
      this.productInfoLoading = true;
      axios.get(`/api/products/${id}`)
        .then(res => {
          if (!res.data.data.id)
          {
            throw res;
            return;
          }
          this.formData = {...res.data.data};
          this.productInfoLoading = false;
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
        });
    },

    onNameChanged(e){
      this.formData.slug = vietnameseNormalize(e.target.value);
    },
    onSubmit() {
      this.$refs.ruleForm.validate(valid => {
        if (!valid)
        {
          return false;
        }

        this.productInfoLoading = true;

        const productId = this.$route.params.id;
        if (productId)
        {
          axios.put(`/api/products/${productId}`, this.formData)
            .then(res => {
              this.$message.success('Đã sửa sản phẩm thành công');
            })
            .catch(err => {
              console.log(err);

              this.$message.error('Thất bại');
            })
            .then(()=>{
              this.productInfoLoading = false;
            });
        }
        else
        {
          axios.post('/api/products', this.formData)
            .then(res => {
              this.formData.id = res.data.data.id;

              if (!this.formData.id)
              {
                throw res;
                return;
              }

              this.$message.success('Đã thêm sản phẩm thành công');
              this.$router.push({ path: `/products/${this.formData.id}/edit` });
            })
            .catch(err => {
              console.log(err);

              this.$message.error('Thất bại');
            })
            .then(()=>{
              this.productInfoLoading = false;
            });
        }
      });
    },
    resetForm() {
      this.$refs.ruleForm.resetFields();
    },
  },
};
</script>
