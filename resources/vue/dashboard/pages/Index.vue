<template>
    <div>
        <a-spin :spinning="loading">
            <a-row :gutter="8">
                <a-col :span="6">
                    <a-card title="Đơn hàng">
                        <a-statistic title="Tổng" :value="statistics.order.count" />
                        <a-statistic title="Chưa hoàn tất" :value="statistics.order.uncompleted_count" />
                        <!-- <a-statistic title="Tổng lãi tháng này (VND)" :value="statistics.transaction.this_month_earning_total" /> -->
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
                        <a-statistic title="Tổng số mặt hàng" :value="statistics.stock.count" />
                        <a-statistic title="Tổng số mặt hàng có sẵn" :value="statistics.stock.avail_count" />
                        <!-- <a-statistic title="Tổng có sẵn" :value="statistics.stock.avail_count_quantity" /> -->
                        <a-statistic title="Tổng giá trị hàng có sẵn trong kho (VND)" :value="statistics.stock.avail_cost_price" />
                        <!-- <a-statistic title="Vốn tháng này (VND)" :value="statistics.stock.this_month_cost_price_total" /> -->
                    </a-card>
                </a-col>
                <a-col :span="6">
                    <a-card title="Người dùng">
                        <a-statistic title="Tổng" :value="statistics.user.count" />
                        <a-statistic title="Admin" :value="statistics.user.admin_count" />
                    </a-card>
                </a-col>
            </a-row>
            <a-card title="Thu chi" style="margin-top: 15px;">
                <a-row :gutter="8">
                    <a-col :span="5" :lg="5" :md="24" :sm="24" :xs="24">
                        <a-statistic title="Tổng Lãi (VND)" :value="statistics.transaction.amount_total" />
                        <a-statistic title="Tổng Lãi tháng này (VND)" :value="statistics.transaction.this_month_amount_total" />
                    </a-col>
                    <a-col :span="19" :lg="19" :md="24" :sm="24" :xs="24">
                        <line-chart :height="150" type="line" :chart-data="datacollection" :options="chartOptions"></line-chart>
                    </a-col>
                </a-row>
            </a-card>
        </a-spin>
    </div>
</template>

<script>
import { number_format } from '../../helpers';

export default {
    components: {
        LineChart: () => import('../utils/LineChart'),
    },
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
                    avail_count_quantity: undefined,
                    cost_price_total: undefined,
                    this_month_cost_price_total: undefined,
                },
                transaction: {
                    amount_total: undefined,
                    this_month_amount_total: undefined,
                },
            },

            datacollection: {},
            chartOptions: null,
        };
    },
    mounted(){
        this.loading = true;

        this.chartOptions = {
            responsive: true,
            plugins: {
                legend: {
                    position: 'top',
                },
                title: {
                    display: true,
                    text: 'Chart.js Line Chart'
                },
            },
            tooltips: {
                callbacks: {
                    label: function(t, d) {
                        // return t.datasetIndex;
                        // console.log(t,d);
                        return number_format(t.yLabel);
                    },
                },
            },
        };

        axios.get('/api/statistics')
            .then(res => {
                this.statistics = {...res.data}

                this.datacollection = {
                    labels: this.statistics.transaction.chart?.map(value => value.ym),
                    datasets: [
                        {
                            label: 'Lãi',
                            backgroundColor: '#ffb1c1',
                            borderColor: '#f87979',
                            data: this.statistics.transaction.chart?.map(value => value.amount)
                        },
                    ]
                };
            })
            .catch(err => {
                if (err.response && err.response.data.message) {
                    this.$message.error(err.response.data.message);
                    return;
                }

                this.$message.error(err.message || 'Thất bại');
            })
            .finally(() => {
                this.loading = false;
            });
    },
    methods: {
        getRandomInt () {
            return Math.floor(Math.random() * (50 - 5 + 1)) + 5
        },
    },
}
</script>
