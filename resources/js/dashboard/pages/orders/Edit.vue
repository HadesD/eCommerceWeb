<template>
  <div>
    <a-spin :spinning="stockInfoLoading">
      <h2>{{ $route.params.id ? `Chỉnh sửa hóa đơn #${$route.params.id}` : 'Tạo hóa đơn đặt hàng' }}</h2>
      <a-form-model
        ref="ruleForm"
        :model="formData"
        :rules="rules"
        :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? labelCol : 0"
        :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? wrapperCol : 0"
        >
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
        <a-card title="Sản phẩm đặt mua" style="margin-bottom:16px;">
          <a slot="extra" @click="() => formData.order_products.push(Object.assign({}, order_product_obj))">
            <a-tooltip title="Thêm sản phẩm">
              <a-button type="primary" icon="plus"></a-button>
            </a-tooltip>
          </a>
          <a-card v-for="(p, pIdx) in formData.order_products" :key="p.id" :title="p.id || 'Sản phẩm'" style="margin-bottom: 16px;">
            <a-popconfirm
              slot="extra"
              title="Chắc chắn muốn xóa?"
              @confirm="() => formData.order_products.splice(pIdx, 1)"
              >
              <a-button type="primary" icon="delete"></a-button>
            </a-popconfirm>
            <a-form-model-item label="Sản phẩm">
              <a-select
                v-model="p.product_id"
                >
                <a-select-option :value="0">
                  Bản nháp
                </a-select-option>
              </a-select>
            </a-form-model-item>
            <a-form-model-item label="Hình thức thanh toán">
              <a-select
                v-model="formData.order_products[pIdx].payment_method"
                >
                <a-select-option :value="1">
                  Trả thẳng 100%
                </a-select-option>
                <a-select-option :value="2">
                  Trả góp
                </a-select-option>
              </a-select>
            </a-form-model-item>
			<a-card title="Xuất kho" style="margin-bottom:16px;">
              <a slot="extra" @click="() => p.order_product_stocks.push(Object.assign({}, order_product_stock_obj))">
                <a-tooltip title="Chọn thêm hàng từ kho">
                  <a-button type="primary" icon="plus"></a-button>
                </a-tooltip>
              </a>
			  <a-card v-for="(ps, psIdx) in p.order_product_stocks" :key="ps.id" :title="ps.id || 'Hàng trong kho'" style="margin-bottom: 16px;">
                <a-popconfirm
                  slot="extra"
                  title="Chắc chắn muốn xóa?"
                  @confirm="() => p.order_product_stocks.splice(psIdx, 1)"
                  >
                  <a-button type="primary" icon="delete"></a-button>
                </a-popconfirm>
                <a-form-model-item label="Hàng">
                  <a-select
                    v-model="ps.stock_id"
                    >
                    <a-select-option :value="0">
                      Bản nháp
                    </a-select-option>
                  </a-select>
                </a-form-model-item>
                <a-form-model-item label="Tổng tiền phải thanh toán">
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
                <a-card title="Thanh toán" style="margin-bottom:16px;">
                  <a slot="extra" @click="() => ps.transactions.push(Object.assign({}, transaction_obj))"><a-button type="primary" icon="plus"></a-button></a>
                  <a-card v-for="(pst, pstIdx) in ps.transactions" :key="pst.id" :title="pst.id || 'Giao dịch'" style="margin-bottom: 16px;">
                    <a-popconfirm
                      slot="extra"
                      title="Chắc chắn muốn xóa?"
                      @confirm="() => ps.transactions.splice(pstIdx, 1)"
                      >
                      <a-button type="primary" icon="delete"></a-button>
                    </a-popconfirm>
                    <a-form-model-item label="Nội dung giao dịch">
                      <a-input
                        v-model="pst.description"
                        placeholder="Trả góp, trả thẳng, thanh toán sản phẩm ABC, vv..vv"
                        type="textarea"
                        />
                    </a-form-model-item>
                    <a-form-model-item label="Số tiền">
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
        <a-card title="Giao dịch thêm" style="margin-bottom:16px;">
          <a slot="extra" @click="() => formData.addon_transactions.push(Object.assign({}, transaction_obj))">
            <a-tooltip title="Thêm giao dịch">
              <a-button type="primary" icon="plus"></a-button>
            </a-tooltip>
          </a>
          <a-card v-for="(item, index) in formData.addon_transactions" :key="item.id" :title="item.id || 'Giao dịch'" style="margin-bottom: 16px;">
            <a-popconfirm
              slot="extra"
              title="Chắc chắn muốn xóa?"
              @confirm="() => formData.addon_transactions.splice(index, 1)"
              >
              <a-button type="primary" icon="delete"></a-button>
            </a-popconfirm>
            <a-form-model-item label="Nội dung giao dịch">
                <a-input
                  v-model="item.description"
                  placeholder="Mã giảm giá, phí ship, v..v"
                  type="textarea"
                  />
            </a-form-model-item>
            <a-form-model-item label="Số tiền">
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

      stockInfoLoading: false,
      formData: {
        id: undefined,
        note: '',
        tax: 0.0,
        customer_id: 0, // TODO: config this
        order_products: [],
        addon_transactions: [],
      },
      rules: {
      },
    }
  },
  mounted() {
    this.formData.id = this.$route.params.id;

    if (this.formData.id)
    {
      this.loadOrder(this.formData.id)
    }
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
          this.formData = {...orderData};
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
              this.formData.id = res.data.data.id;

              if (!this.formData.id)
              {
                throw res;
                return;
              }

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
  },
}
</script>
