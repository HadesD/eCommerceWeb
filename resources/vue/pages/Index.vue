<template>
    <a-row :gutter="16">
        <a-col :sm="{ span: 24 }" :md="{ span: 12 }" :lg="{ span: 6 }">
            <a-card style="margin-bottom: 10px;">
                <a-card-meta title="Miễn phí vận chuyển" description="Đơn hàng từ 300k">
                    <template #avatar>
                        <DollarCircleFilled style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :sm="{ span: 24 }" :md="{ span: 12 }" :lg="{ span: 6 }">
            <a-card style="margin-bottom: 10px;">
                <a-card-meta title="Miễn phí đổi trả" description="Trong vòng 7 ngày">
                    <template #avatar>
                        <TransactionOutlined style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :sm="{ span: 24 }" :md="{ span: 12 }" :lg="{ span: 6 }">
            <a-card style="margin-bottom: 10px;">
                <a-card-meta title="Bảo hành dài hạn" description="Lên tới 12 tháng">
                    <template #avatar>
                        <ClockCircleOutlined style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :sm="{ span: 24 }" :md="{ span: 12 }" :lg="{ span: 6 }">
            <a-card style="margin-bottom: 10px;">
                <a-card-meta title="Tặng kèm hấp dẫn" description="Quà tặng đa dạng">
                    <template #avatar>
                        <GiftOutlined style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
    </a-row>
</template>
<script>
import {
    DollarCircleFilled, TransactionOutlined,
    ClockCircleOutlined, GiftOutlined,
} from '@ant-design/icons-vue';

export default {
    components: {
        DollarCircleFilled, TransactionOutlined,
        ClockCircleOutlined, GiftOutlined,
    },
}
</script>
