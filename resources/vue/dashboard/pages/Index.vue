<template>
    <div>
        <a-row :gutter="8" style="margin-bottom: 15px;">
            <a-col :offset="20" :span="4">
                <a-button type="primary" :loading="loading" style="float: right;" @click="() => loadStatistic()">
                    <template #icon><ReloadOutlined /></template>
                </a-button>
            </a-col>
        </a-row>
        <a-spin :spinning="loading">
            <a-row :gutter="8">
                <a-col :span="6" :lg="6" :md="12" :sm="24" :xs="24">
                    <a-card title="Đơn hàng">
                        <a-statistic title="Tổng" :value="statistics.order.count" />
                        <a-statistic title="Chưa hoàn tất" :value="statistics.order.uncompleted_count" />
                    </a-card>
                </a-col>
                <a-col :span="6" :lg="6" :md="12" :sm="24" :xs="24">
                    <a-card title="Sản phẩm">
                        <a-statistic title="Tổng" :value="statistics.product.count" />
                        <a-statistic title="Đang bán" :value="statistics.product.selling_count" />
                    </a-card>
                </a-col>
                <a-col :span="6" :lg="6" :md="12" :sm="24" :xs="24">
                    <a-card title="Kho hàng">
                        <a-statistic title="Tổng số mặt hàng" :value="statistics.stock.count" />
                        <a-statistic title="Tổng số mặt hàng có sẵn" :value="statistics.stock.avail_count" />
                        <a-statistic title="Tổng giá trị hàng có sẵn trong kho" :value="statistics.stock.avail_cost_price" suffix="₫" />
                    </a-card>
                </a-col>
                <a-col :span="6" :lg="6" :md="12" :sm="24" :xs="24">
                    <a-card title="Người dùng">
                        <a-statistic title="Tổng" :value="statistics.user.count" />
                        <a-statistic title="Admin" :value="statistics.user.admin_count" />
                    </a-card>
                </a-col>
            </a-row>
            <a-card title="Thu chi" style="margin-top: 15px;">
                <a-row :gutter="8">
                    <a-col :span="5" :lg="5" :md="24" :sm="24" :xs="24">
                        <a-statistic title="Vốn hiện tại" :value="statistics.transaction.funds" suffix="₫" />
                        <a-statistic title="Tổng Lãi" :value="statistics.transaction.amount_total" suffix="₫" />
                        <a-statistic title="Tổng Lãi tháng này" :value="statistics.transaction.this_month_amount_total" suffix="₫" />
                        <a-statistic title="Tổng khách đang Nợ" :value="statistics.transaction.remaining_need_paid_total" suffix="₫" />
                        <a-statistic title="Tổng Lãi Thực trước nợ" :value="statistics.transaction.real_amount_total_before_debt" suffix="₫" />
                        <a-statistic title="Tổng Lãi Thực sau nợ" :value="statistics.transaction.real_amount_total_after_debt" suffix="₫" />
                    </a-col>
                    <a-col :span="19" :lg="19" :md="24" :sm="24" :xs="24">
                        <line-chart :height="150" type="line" :chartData="datacollectionNear30days" :options="chartOptionsNear30days" />
                        <line-chart :height="150" type="line" :chartData="datacollectionTotal" :options="chartOptionsTotal" />
                    </a-col>
                </a-row>
            </a-card>
        </a-spin>
    </div>
</template>

<script>
import { defineAsyncComponent } from 'vue';
import {
    ReloadOutlined,
} from '@ant-design/icons-vue';

import { number_format } from '../../helpers';
import RequestRepository from '../utils/RequestRepository';

export default {
    components: {
        LineChart: defineAsyncComponent(() => import('../utils/LineChart')),
        ReloadOutlined,
    },
    data(){
        return {
            loading: false,

            statistics: {
                order: {},
                user: {},
                product: {},
                stock: {},
                transaction: {},
            },

            datacollectionTotal: {},
            chartOptionsTotal: null,
            datacollectionNear30days: {},
            chartOptionsNear30days: null,
        };
    },
    mounted(){
        this.chartOptionsTotal = {
            responsive: true,
            plugins: {
                legend: {
                    position: 'top',
                },
            },
            tooltips: {
                callbacks: {
                    label: function(t, d) {
                        return number_format(t.yLabel);
                    },
                },
            },
        };

        this.chartOptionsNear30days = {
            responsive: true,
            plugins: {
                legend: {
                    position: 'top',
                },
            },
            tooltips: {
                callbacks: {
                    label: function(t, d) {
                        return number_format(t.yLabel);
                    },
                },
            },
        };

        this.loadStatistic();
    },
    methods: {
        loadStatistic() {
            this.loading = true;

            RequestRepository.get('/statistics')
                .then(res => {
                    this.statistics = {...res.data};

                    const chart_total = this.statistics.transaction.chart_total;
                    this.datacollectionTotal = {
                        labels: chart_total.map(value => value.ym),
                        datasets: [
                            {
                                label: 'Lãi Tháng',
                                backgroundColor: '#ffb1c1',
                                borderColor: '#f87979',
                                data: chart_total.map(value => value.amount),
                            },
                        ]
                    };

                    const chart_near_30_days = this.statistics.transaction.chart_near_30_days;
                    this.datacollectionNear30days = {
                        labels: chart_near_30_days.map(value => value.ymd),
                        datasets: [
                            {
                                label: 'Lãi Ngày',
                                backgroundColor: '#ffb1c1',
                                borderColor: '#f87979',
                                data: chart_near_30_days.map(value => value.amount),
                            },
                        ]
                    };
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(() => {
                    this.loading = false;
                });
        },
    },
}
</script>
