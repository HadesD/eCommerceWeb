<template>
  <div>
    <a-spin :spinning="orderInfoLoading">
      <h2>{{ $route.params.id ? `Chỉnh sửa hóa đơn #${$route.params.id}` : 'Tạo hóa đơn đặt hàng' }}</h2>
      <a-form-model
        ref="ruleForm"
        :model="formData"
        :rules="rules"
        :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? labelCol : 0"
        :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? wrapperCol : 0"
        >
        <a-form-model-item label="Trạng thái" prop="status">
          <a-select
            v-model="formData.status"
            >
            <a-select-option :value="0">
              Đang xử lí
            </a-select-option>
            <a-select-option :value="10">
              Đã hủy bỏ
            </a-select-option>
            <a-select-option :value="50">
              Đã thanh toán
            </a-select-option>
            <a-select-option :value="51">
              Đang thanh toán
            </a-select-option>
            <a-select-option :value="100">
              Đang ship
            </a-select-option>
            <a-select-option :value="200">
              Hoàn tất
            </a-select-option>
          </a-select>
        </a-form-model-item>
        <a-form-model-item label="Khách hàng" ref="customer_id" prop="customer_id">
          <a-input
            v-model="formData.customer_id"
            />
        </a-form-model-item>
        <a-form-model-item label="Ghi chú">
          <a-textarea
            v-model="formData.note"
            />
        </a-form-model-item>
        <a-card title="Sản phẩm đặt mua" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#9800ab',color:'#FFF'}">
          <a
            slot="extra"
            @click="() => {formData.order_products.push(Object.assign({}, order_product_obj));}"
            >
            <a-tooltip title="Thêm sản phẩm">
              <a-button type="primary" icon="plus"></a-button>
            </a-tooltip>
          </a>
          <a-card v-for="(p, pIdx) in formData.order_products" :key="p.id" :title="p.id || 'Sản phẩm #'+pIdx" style="margin-bottom: 16px;" :headStyle="{backgroundColor:'#f18e1f',color:'#FFF'}">
            <a-popconfirm
              slot="extra"
              title="Chắc chắn muốn xóa?"
              @confirm="() => formData.order_products.splice(pIdx, 1)"
              >
              <a-button type="primary" icon="delete"></a-button>
            </a-popconfirm>
            <a-form-model-item label="Sản phẩm" :rules="{required:true, trigger: 'blur'}"
              :prop="'order_products.'+pIdx+'.product_id'"
              >
              <a-tree-select
                :tree-data="productData"
                :load-data="loadCategoryProducts"
                placeholder="Please select"
                v-model="p.product_id"
                />
            </a-form-model-item>
            <a-form-model-item label="Hình thức thanh toán" :rules="{required:true, trigger: 'blur'}">
              <a-select
                v-model="p.payment_method"
                >
                <a-select-option :value="1">
                  Trả thẳng 100%
                </a-select-option>
                <a-select-option :value="2">
                  Trả góp
                </a-select-option>
              </a-select>
            </a-form-model-item>
			<a-card title="Xuất kho" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#680075',color:'#FFF'}">
              <a slot="extra" @click="() => p.order_product_stocks.push(Object.assign({}, order_product_stock_obj))">
                <a-tooltip title="Chọn thêm hàng từ kho">
                  <a-button type="primary" icon="plus"></a-button>
                </a-tooltip>
              </a>
			  <a-card v-for="(ps, psIdx) in p.order_product_stocks" :key="ps.id" :title="ps.id || 'Hàng trong kho #'+psIdx" style="margin-bottom: 16px;">
                <a-popconfirm
                  slot="extra"
                  title="Chắc chắn muốn xóa?"
                  @confirm="() => p.order_product_stocks.splice(psIdx, 1)"
                  >
                  <a-button type="primary" icon="delete"></a-button>
                </a-popconfirm>
                <a-form-model-item label="Hàng" :rules="{required:true, trigger: 'blur'}"
                  :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.stock_id'"
                  >
                  <a-tree-select
                    :tree-data="stockData"
                    :load-data="loadCategoryStocks"
                    placeholder="Please select"
                    v-model="ps.stock_id"
                    />
                </a-form-model-item>
                <a-form-model-item label="Tổng tiền phải thanh toán" :rules="{required:true, trigger: 'blur'}" :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.amount'">
                  <a-input-number
                    v-model="ps.amount"
                    :formatter="value => new Intl.NumberFormat().format(value)"
                    :parser="value => value.replace(/\$\s?|(\.*)/g, '')"
                    style="width: 100%;"
                    :min="0"
                    :max="2000000000"
                    >
                  </a-input-number>
                </a-form-model-item>
                <a-card title="Thanh toán" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#9800ab',color:'#FFF'}">
                  <a slot="extra" @click="() => ps.transactions.push(Object.assign({}, transaction_obj))">
                    <a-tooltip title="Thêm giao dịch">
                      <a-button type="primary" icon="plus"></a-button>
                    </a-tooltip>
                  </a>
                  <a-card v-for="(pst, pstIdx) in ps.transactions" :key="pst.id" :title="pst.id || 'Giao dịch #'+pstIdx" style="margin-bottom: 16px;" :headStyle="{backgroundColor:'#f18e1f',color:'#FFF'}">
                    <a-popconfirm
                      slot="extra"
                      title="Chắc chắn muốn xóa?"
                      @confirm="() => ps.transactions.splice(pstIdx, 1)"
                      >
                      <a-button type="primary" icon="delete"></a-button>
                    </a-popconfirm>
                    <a-form-model-item label="Nội dung giao dịch" :rules="{required:true, trigger: 'blur'}" :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.transactions.'+pstIdx+'.description'">
                      <a-input
                        v-model="pst.description"
                        placeholder="Trả góp, trả thẳng, thanh toán sản phẩm ABC, vv..vv"
                        type="textarea"
                        />
                    </a-form-model-item>
                    <a-form-model-item label="Số tiền" :rules="{required:true, trigger: 'blur'}" :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.transactions.'+pstIdx+'.amount'">
                      <a-input-number
                        v-model="pst.amount"
                        :formatter="value => new Intl.NumberFormat().format(value)"
                        :parser="value => value.replace(/\$\s?|(\.*)/g, '')"
                        style="width: 100%;"
                        :min="-2000000000"
                        :max="2000000000"
                        >
                      </a-input-number>
                    </a-form-model-item>
                    <a-form-model-item label="Ngày thanh toán">
                      <a-date-picker
                        v-model="pst.paid_date"
                        format="YYYY-MM-DD HH:mm:ss"
                        show-time
                        type="date"
                        />
                    </a-form-model-item>
                  </a-card>
                </a-card>
              </a-card>
            </a-card>
          </a-card>
        </a-card>
        <a-card title="Giao dịch thêm" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#9800ab',color:'#FFF'}">
          <a slot="extra" @click="() => formData.transactions.push(Object.assign({}, transaction_obj))">
            <a-tooltip title="Thêm giao dịch">
              <a-button type="primary" icon="plus"></a-button>
            </a-tooltip>
          </a>
          <a-card
            v-for="(item, index) in formData.transactions" :key="item.id"
            :title="item.id || 'Giao dịch #'+index"
            style="margin-bottom: 16px;"
            :headStyle="{backgroundColor:'#f18e1f',color:'#FFF'}">
            <a-popconfirm
              slot="extra"
              title="Chắc chắn muốn xóa?"
              @confirm="() => formData.transactions.splice(index, 1)"
              >
              <a-button type="primary" icon="delete"></a-button>
            </a-popconfirm>
            <a-form-model-item label="Nội dung giao dịch" :rules="{required:true, trigger: 'blur'}" :prop="'transactions.'+index+'.description'">
                <a-input
                  v-model="item.description"
                  placeholder="Mã giảm giá, phí ship, v..v"
                  type="textarea"
                  />
            </a-form-model-item>
            <a-form-model-item label="Số tiền" :rules="{required:true, trigger: 'blur'}" :prop="'transactions.'+index+'.amount'">
              <a-input-number
                v-model="item.amount"
                :formatter="value => new Intl.NumberFormat().format(value)"
                :parser="value => value.replace(/\$\s?|(\.*)/g, '')"
                style="width: 100%;"
                :min="-2000000000"
                :max="2000000000"
                >
              </a-input-number>
            </a-form-model-item>
            <a-form-model-item label="Ngày thanh toán">
              <a-date-picker
                v-model="item.paid_date"
                format="YYYY-MM-DD HH:mm:ss"
                show-time
                type="date"
                />
            </a-form-model-item>
          </a-card>
        </a-card>
        <a-form-model-item :wrapper-col="{ span: 14, offset: 4 }">
          <a-button type="primary" @click="onSubmit">
            {{ $route.params.id ? 'Sửa' : 'Tạo đơn' }}
          </a-button>
          <a-button style="margin-left: 10px;" @click="resetForm">Reset</a-button>
        </a-form-model-item>
      </a-form-model>
    </a-spin>
  </div>
</template>

<script>

export default {
  data() {
    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },

      orderInfo: {},
      categories: [],
      productData: [],
      stockData: [],

      orderInfoLoading: false,
      formData: {
        id: undefined,
        note: '',
        customer_id: 0, // TODO: config this
        order_products: [],
        status: undefined,
        transactions: [],
      },
      rules: {
        status: {required: true,},
      },
    }
  },
  mounted() {
    this.formData.id = this.$route.params.id;

    this.loadCategoriesTree();

    if (this.formData.id)
    {
      this.loadOrder(this.formData.id)
    }
  },
  watch: {
    $route (to, from){
      if (!to.params.id)
      {
        this.formData.order_products.forEach((p) => {
          p.id = undefined;
          p.order_product_stocks = [];
        });

        this.formData.transactions.forEach((t) => {
          t.id = undefined;
        });
      }
      else
      {
        this.loadOrder(this.formData.id)
      }
    },
  },
  computed: {
    transaction_obj() {
      return {
        id: undefined,
        description: '',
        amount: undefined,
        paid_date: undefined,
      }
    },
    order_product_obj() {
      return {
        id: undefined,
        order_id: undefined,
        product_id: undefined,
        payment_method: 1,
        order_product_stocks: [],
      }
    },
    order_product_stock_obj() {
      return {
        id: undefined,
        order_product_id: undefined,
        stock_id: undefined,
        amount: undefined,
        transactions: [],
      }
    },
  },
  methods: {
    // CategoriesTree
    loadCategoriesTree(){
      // this.categoriesTreeLoading = true;
      axios.get('/api/categories')
        .then(res => {
          this.categories = res.data.data.sort((a, b) => a.parent_id - b.parent_id);

          const len = this.categories.length;
          for (let i = 0; i < len; i++)
          {
            const elm = this.categories[i];
            if (elm.parent_id !== 0)
            {
              break;
            }

            const newOtp = {
              isLeaf: false,
              selectable: false,
              value: 'cat-'+elm.id,
              title: '[+] ' + elm.name,
              meta: {category_id: elm.id},
            };

            this.productData.push(Object.assign({}, newOtp));
            this.stockData.push(Object.assign({}, newOtp));
          }
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
          // this.categoriesTreeLoading = false;
        });
    },

    loadOrder(id){
      this.orderInfoLoading = true;
      axios.get(`/api/orders/${id}`)
        .then(res => {
          const orderData = res.data.data;
          if (!orderData.id)
          {
            throw res;
            return;
          }

          this.orderInfo = orderData;

          this.formData = {...orderData};

          // Load tree

          this.orderInfoLoading = false;
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
        });
    },

    onSubmit() {
      this.$refs.ruleForm.validate(valid => {
        if (!valid)
        {
          return false;
        }

        this.orderInfoLoading = true;

        const orderId = this.$route.params.id;
        if (orderId)
        {
          axios.put(`/api/orders/${orderId}`, this.formData)
            .then(res => {
              const orderData = res.data.data;
              this.formData.id = res.data.data.id;

              if (!this.formData.id)
              {
                throw res;
                return;
              }

              this.formData = {...orderData};

              this.$message.success('Đã sửa sản phẩm thành công');
            })
            .catch(err => {
              console.log(err);

              this.$message.error('Thất bại');
            })
            .then(()=>{
              this.orderInfoLoading = false;
            });
        }
        else
        {
          axios.post('/api/orders', this.formData)
            .then(res => {
              const orderData = res.data.data;
              this.formData.id = res.data.data.id;

              if (!this.formData.id)
              {
                throw res;
                return;
              }

              this.formData = {...orderData};

              this.$message.success('Đã thêm sản phẩm thành công');
              this.$router.push({ path: `/orders/${this.formData.id}/edit` });
            })
            .catch(err => {
              console.log(err);

              this.$message.error('Thất bại');
            })
            .then(()=>{
              this.orderInfoLoading = false;
            });
        }
      });
    },
    resetForm() {
      this.$refs.ruleForm.resetFields();
    },

    loadCategoryProducts(treeNode) {
      const targetOption = treeNode.dataRef;

      const category_id = targetOption.meta.category_id;

      return axios.get(`/api/products?category_id=${category_id}&all=true`)
        .then(res => {
          const sData = res.data.data;

          targetOption.children = [];

          // Find category
          const len = this.categories.length;
          for (let i = 0; i < len; i++)
          {
            const elm = this.categories[i];
            if (elm.parent_id === category_id)
            {
              const newOtp = {
                isLeaf: false,
                selectable: false,
                value: 'cat-'+elm.id,
                title: '[+] ' + elm.name,
                meta: {category_id: elm.id},
              };
              targetOption.children.push(Object.assign({}, newOtp));
            }
          }

          for (let i = 0; i < sData.length; i++)
          {
            const elm = sData[i];
            const newOtp = {
              isLeaf: true,
              title: elm.id +'. ' + elm.name + ' ('+elm.idi+') ' + ' ('+(new Intl.NumberFormat().format(elm.price))+' VND)',
              value: elm.id,
            };
            targetOption.children.push(newOtp);
          }

          targetOption.disabled = targetOption.children.length === 0;
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
          this.productData = [...this.productData];
        });
    },

    loadCategoryStocks(treeNode) {
      const targetOption = treeNode.dataRef;

      const category_id = targetOption.meta.category_id;

      return axios.get(`/api/stocks?category_id=${category_id}&all=true`)
        .then(res => {
          const sData = res.data.data;

          targetOption.children = [];

          // Find category
          const len = this.categories.length;
          for (let i = 0; i < len; i++)
          {
            const elm = this.categories[i];
            if (elm.parent_id === category_id)
            {
              const newOtp = {
                isLeaf: false,
                selectable: false,
                value: 'cat-'+elm.id,
                title: '[+] ' + elm.name,
                meta: {category_id: elm.id},
              };
              targetOption.children.push(Object.assign({}, newOtp));
            }
          }

          for (let i = 0; i < sData.length; i++)
          {
            const elm = sData[i];
            const newOtp = {
              isLeaf: true,
              disabled: elm.status !== 0,
              title: elm.id +'. ' + elm.name + ' ('+(new Intl.NumberFormat().format(elm.cost_price))+' VND)',
              value: elm.id,
            };
            targetOption.children.push(newOtp);
          }

          targetOption.disabled = targetOption.children.length === 0;
        })
        .catch(err => {
          console.log(err);

          this.$message.error('Thất bại');
        })
        .then(()=>{
          this.stockData = [...this.stockData];
        });
    },
  },
}
</script>
