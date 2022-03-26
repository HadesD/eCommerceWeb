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
import { computed, defineAsyncComponent, onMounted, reactive, ref } from 'vue';
import {
    ReloadOutlined,
} from '@ant-design/icons-vue';

import { number_format, showErrorRequestApi } from '~/helpers';
import RequestApiRepository from '~/utils/RequestApiRepository';

export default {
    components: {
        LineChart: defineAsyncComponent(() => import('../utils/LineChart')),
        ReloadOutlined,
    },

    setup() {
        const loading = ref(false);
        const statistics = reactive({
            order: {},
            user: {},
            product: {},
            stock: {},
            transaction: {},
        });
        const datacollectionTotal = computed(() => {
            const chart_total = statistics.transaction?.chart_total;

            return {
                labels: chart_total?.map(value => value.ym),
                datasets: [
                    {
                        label: 'Lãi Tháng',
                        backgroundColor: '#ffb1c1',
                        borderColor: '#f87979',
                        data: chart_total?.map(value => value.amount),
                    },
                ],
            };
        });
        const datacollectionNear30days = computed(() => {
            const chart_near_30_days = statistics.transaction?.chart_near_30_days;

            return {
                labels: chart_near_30_days?.map(value => value.ymd),
                datasets: [
                    {
                        label: 'Lãi Ngày',
                        backgroundColor: '#ffb1c1',
                        borderColor: '#f87979',
                        data: chart_near_30_days?.map(value => value.amount),
                    },
                ],
            };
        });

        const loadStatistic = () => {
            loading.value = true;

            RequestApiRepository.get('/statistics')
                .then(res => {
                    for (let i in res.data) {
                        statistics[i] = res.data[i];
                    }
                })
                .finally(() => {
                    loading.value = false;
                });
        };

        onMounted(() => {
            loadStatistic();
        });

        return {
            loading,

            loadStatistic,
            statistics,

            datacollectionTotal,
            chartOptionsTotal: {
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
            },
            datacollectionNear30days,
            chartOptionsNear30days: {
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
            },
        };
    },
}
</script>
