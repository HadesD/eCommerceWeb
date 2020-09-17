<template>
  <div>
    <a-spin :spinning="stockInfoLoading">
      <h2>{{ $route.params.id ? `Sửa hàng trong kho #${$route.params.id}` : 'Nhập hàng mới vào kho' }}</h2>
      <a-form-model
        ref="ruleForm"
        :model="formData"
        :rules="rules"
        :label-col="labelCol"
        :wrapper-col="wrapperCol"
        >
        <a-form-model-item label="Tên sản phẩm" ref="name" prop="name">
          <a-input
            v-model="formData.name"
            />
        </a-form-model-item>
        <a-form-model-item label="Id/imei/mã phân biệt" ref="idi" prop="idi">
          <a-input
            v-model="formData.idi"
            @blur="() => $refs.idi.onFieldBlur()"
            />
        </a-form-model-item>
        <a-form-model-item label="Giá lúc nhập" ref="cost_price" prop="cost_price">
          <a-input-number
            v-model="formData.cost_price"
            @blur="() => $refs.cost_price.onFieldBlur()"
            :formatter="value => new Intl.NumberFormat().format(value)"
            :parser="value => value.replace(/\$\s?|(\.*)/g, '')"
            style="width: 100%;"
            >
          </a-input-number>
        </a-form-model-item>
        <a-form-model-item label="Trạng thái" ref="status" prop="status">
          <a-select
            v-model="formData.status"
            @blur="() => $refs.status.onFieldBlur()"
            >
            <a-select-option :value="0">
              Có sẵn
            </a-select-option>
            <a-select-option :value="1" :disabled="$route.params.id ? false : true">
              Đã bán
            </a-select-option>
            <a-select-option :value="2">
              Hỏng / Lỗi
            </a-select-option>
          </a-select>
        </a-form-model-item>
        <a-form-model-item label="Ghi chú">
          <a-textarea
            v-model="formData.note"
            />
        </a-form-model-item>
        <a-form-model-item label="Ngày nhập" ref="in_date" prop="in_date">
          <a-date-picker
            v-model="formData.in_date"
            show-time
            type="date"
            @blur="() => $refs.in_date.onFieldBlur()"
            />
        </a-form-model-item>
        <a-form-model-item :wrapper-col="{ span: 14, offset: 4 }">
          <a-button type="primary" @click="onSubmit">
            {{ $route.params.id ? 'Sửa' : 'Nhập kho' }}
          </a-button>
          <a-button style="margin-left: 10px;" @click="resetForm">Reset</a-button>
        </a-form-model-item>
      </a-form-model>
    </a-spin>
  </div>
</template>

<script>
import axios from 'axios';

export default {
  data() {
    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },

      stockInfoLoading: false,
      formData: {
        id: undefined,
        name: '',
        idi: '',
        cost_price: undefined,
        status: 0,
        in_date: '',
        note: '',
      },
      rules: {
        idi: [
          { required: true, trigger: 'blur' },
        ],
        cost_price: [
          { required: true, trigger: 'blur' },
        ],
        status: [
          { required: true, trigger: 'blur' },
        ],
      },
    }
  },
  mounted() {
    this.formData.id = this.$route.params.id;

    if (this.formData.id)
    {
      this.loadStock(this.formData.id)
    }
  },
  methods: {
    loadStock(id){
      this.stockInfoLoading = true;
      axios.get(`/api/stocks/${id}`)
        .then(res => {
          if (!res.data.data.id)
          {
            throw res;
            return;
          }
          this.formData = {...res.data.data};
          this.stockInfoLoading = false;
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

        this.stockInfoLoading = true;

        const stockId = this.$route.params.id;
        if (stockId)
        {
          axios.put(`/api/stocks/${stockId}`, this.formData)
            .then(res => {
              this.$message.success('Đã sửa sản phẩm thành công');
            })
            .catch(err => {
              console.log(err);

              this.$message.error('Thất bại');
            })
            .then(()=>{
              this.stockInfoLoading = false;
            });
        }
        else
        {
          axios.post('/api/stocks', this.formData)
            .then(res => {
              this.formData.id = res.data.data.id;

              if (!this.formData.id)
              {
                throw res;
                return;
              }

              this.$message.success('Đã thêm sản phẩm thành công');
              this.$router.push({ path: `/stocks/${this.formData.id}/edit` });
            })
            .catch(err => {
              console.log(err);

              this.$message.error('Thất bại');
            })
            .then(()=>{
              this.stockInfoLoading = false;
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
