<template>
    <a-row type="flex" :gutter="16">
        <a-col :flex="1">
            <a-card>
                <a-card-meta title="Miễn phí vận chuyển" description="Đơn hàng từ 300k">
                    <template #avatar>
                        <DollarCircleFilled style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :flex="1">
            <a-card>
                <a-card-meta title="Miễn phí đổi trả" description="Trong vòng 7 ngày">
                    <template #avatar>
                        <TransactionOutlined style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :flex="1">
            <a-card>
                <a-card-meta title="Bảo hành dài hạn" description="Lên tới 12 tháng">
                    <template #avatar>
                        <ClockCircleOutlined style="font-size: 50px;" />
                    </template>
                </a-card-meta>
            </a-card>
        </a-col>
        <a-col :flex="1">
            <a-card>
                <a-card-meta title="Tặng kèm hấp dẫn" description="Khi mua hàng">
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
