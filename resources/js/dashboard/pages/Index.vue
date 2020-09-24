<template>
  <div>
    <a-spin :spinning="loading">
      <a-row :gutter="8">
        <a-col :span="6">
          <a-card title="Đơn hàng">
            <a-statistic title="Tổng" :value="statistics.order.count" />
            <a-statistic title="Chưa hoàn tất" :value="statistics.order.uncompleted_count" />
            <a-statistic title="Tổng thu (VND)" :value="statistics.transaction.amount_total" />
            <a-statistic title="Tổng thu tháng này (VND)" :value="statistics.transaction.this_month_amount_total" />
          </a-card>
        </a-col>
        <a-col :span="6">
          <a-card title="Sản phẩm">
            <a-statistic title="Tổng" :value="statistics.product.count" />
              <a-statistic title="Đang bán" :value="statistics.product.selling_count" />
          </a-card>
        </a-col>
        <a-col :span="6">
          <a-card title="Kho hàng">
            <a-statistic title="Tổng" :value="statistics.stock.count" />
            <a-statistic title="Có sẵn" :value="statistics.stock.avail_count" />
            <a-statistic title="Tổng vốn (VND)" :value="statistics.stock.cost_price_total" />
            <a-statistic title="Vốn tháng này (VND)" :value="statistics.stock.this_month_cost_price_total" />
          </a-card>
        </a-col>
        <a-col :span="6">
          <a-card title="Người dùng">
            <a-statistic title="Tổng" :value="statistics.user.count" />
              <a-statistic title="Admin" :value="statistics.user.admin_count" />
          </a-card>
        </a-col>
      </a-row>
    </a-spin>
  </div>
</template>

<script>
export default {
  data(){
    return {
      loading: false,

      statistics: {
        order: {
          count: undefined,
          uncompleted_count: undefined,
        },
        user: {
          count: undefined,
          admin_count: undefined,
        },
        product: {
          count: undefined,
          selling_count: undefined,
        },
        stock: {
          count: undefined,
          avail_count: undefined,
          cost_price_total: undefined,
		  this_month_cost_price_total: undefined,
        },
        transaction: {
          amount_total: undefined,
          this_month_amount_total: undefined,
        },
      }
    }
  },
  mounted(){
    this.loading = true;
    axios.get('/api/statistics')
      .then(res => {
        console.log(res);
        this.statistics = {...res.data}
      })
      .catch(res => {

      })
      .then(() => {
        this.loading = false;
      });
  },
}
</script>
