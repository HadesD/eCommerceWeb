<template>
    <a-spin :spinning="orderInfoLoading">
        <a-page-header
            :title="id ? 'Chỉnh sửa hóa đơn' : 'Tạo hóa đơn'"
            :sub-title="id ? `#${id}` : false"
            :ghost="false"
        >
            <template #tags>
                <a-tooltip title="Lấy dữ liệu mới nhất" v-if="id">
                    <a-button type="primary" size="small" :loading="orderInfoLoading" @click="() => loadOrder(id)">
                        <template #icon><ReloadOutlined /></template>
                    </a-button>
                </a-tooltip>
                <a-tooltip title="Xoá toàn bộ dữ liệu đang nhập" v-if="!orderId">
                    <a-popconfirm title="Xác nhận reset toàn bộ dữ liệu đang nhập?" @confirm="() => this.formData.id = (this.formData.id === undefined) ? null : undefined">
                        <a-button type="primary" danger size="small">
                            <template #icon><DeleteOutlined /></template>
                        </a-button>
                    </a-popconfirm>
                </a-tooltip>
            </template>
            <template #extra>
                <template v-if="id">
                    <a-tooltip title="In hoá đơn">
                        <a-button type="primary" @click="() => printOrder()">
                            <template #icon><PrinterOutlined /></template>
                        </a-button>
                    </a-tooltip>
                </template>
            </template>
            <a-descriptions size="small" :column="1" v-if="id">
                <a-descriptions-item label="Ngày tạo">
                    <span>{{ date_format(orderInfo.created_at) }}</span>
                </a-descriptions-item>
                <a-descriptions-item label="Ngày cập nhật">
                    <span>{{ date_format(orderInfo.updated_at) }}</span>
                </a-descriptions-item>
                <a-descriptions-item label="Người cập nhật" v-if="orderInfo.updated_user">
                    <span>{{ orderInfo.updated_user_id }}. {{ orderInfo.updated_user.name }}</span>
                    <a-button size="small" @click="() => { currentUserId = orderInfo.updated_user_id; userEditPageVisible = true; }">
                        <template #icon><SearchOutlined /></template>
                    </a-button>
                </a-descriptions-item>
            </a-descriptions>
        </a-page-header>
        <a-form
            ref="ruleForm"
            :model="formData"
            :rules="rules"
            @finish="onFinish"
            :label-col="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) === -1) ? { span: 4 } : {}"
            :wrapper-col="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) === -1) ? { span: 14 } : {}"
        >
            <a-form-item label="Trạng thái" name="status">
                <a-select v-model:value="formData.status">
                    <a-select-option v-for="stsCode in Object.keys(configOrderStatus)" :key="stsCode" :value="parseInt(stsCode)">{{ configOrderStatus[stsCode].name }}</a-select-option>
                </a-select>
            </a-form-item>
            <a-form-item
                label="Ngày xuất đơn"
                name="deal_date"
                help="Ngày xuât đơn cho khách hàng"
                :rules="{required:true}"
            >
                <a-date-picker
                    v-model:value="formData.deal_date"
                    format="YYYY-MM-DD HH:mm:ss"
                    show-time
                    type="date"
                    :disabledDate="disabledLastMonthAndTomorrow"
                />
            </a-form-item>
            <a-form-item
                label="Khách hàng"
                name="customer_id"
                :help="orderInfo.customer ? `Đang chọn: #${orderInfo.customer.id}. ${orderInfo.customer.name} (Phone: ${orderInfo.customer.phone || 'Chưa có'})` : false"
            >
                <a-row :gutter="8">
                    <a-col :span="12">
                        <a-input-search v-model:value="formData.customer_id" readOnly @search="() => { currentUserId = formData.customer_id; userEditPageVisible = true; }">
                            <template #enterButton>
                                <a-button>
                                    <template #icon><SearchOutlined /></template>
                                </a-button>
                            </template>
                        </a-input-search>
                    </a-col>
                    <a-col :span="8">
                        <a-tooltip title="Chọn từ danh sách">
                            <a-button type="primary" @click="() => {
                                userIndexPageFinish = (recordData) => {
                                    formData.customer_id = recordData.id;
                                    orderInfo.customer = recordData;
                                };
                                userIndexPageVisible = true;
                            }">
                                <template #icon><UserOutlined /></template> Chọn
                            </a-button>
                        </a-tooltip>
                    </a-col>
                </a-row>
            </a-form-item>
            <a-form-item label="Ghi chú" name="note">
                <a-textarea v-model:value="formData.note" placeholder="Tên khách hàng, loại thanh toán, chi phí sinh hoạt, lương nhân viên, v..v" />
            </a-form-item>
            <a-card title="Sản phẩm đặt mua" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#9800ab',color:'#FFF'}">
                <template #extra>
                    <a @click="() => formData.order_products.push(cloneDeep(order_product_obj))">
                        <a-tooltip title="Thêm sản phẩm">
                            <a-button type="primary">
                                <template #icon><PlusOutlined /></template>
                            </a-button>
                        </a-tooltip>
                    </a>
                </template>
                <a-card :title="`Sản phẩm #${pIdx}`" v-for="(op, pIdx) in formData.order_products" :key="`op-${op.id || Math.random()}`" style="margin-bottom: 16px;" :headStyle="{backgroundColor:'#f18e1f',color:'#FFF'}" size="small">
                    <template #extra>
                        <a-popconfirm v-if="!op.id" title="Chắc chắn muốn xóa?" @confirm="() => formData.order_products.splice(pIdx, 1)">
                            <a-button type="primary" danger>
                                <template #icon><DeleteOutlined /></template>
                            </a-button>
                        </a-popconfirm>
                    </template>
                    <a-form-item
                        label="Sản phẩm"
                        :rules="{required:true,message:'Không được để trống'}"
                        :name="['order_products', pIdx, 'product_id']"
                        :help="op.product ? `Đang chọn: #${op.product.id}. ${op.product.name} (${number_format(op.product.price)} ₫)` : false"
                    >
                        <a-row :gutter="8">
                            <a-col :span="12">
                                <a-input-search v-model:value="op.product_id" readOnly @search="() => {
                                    currentProductId = op.product_id;
                                    productEditPageVisible = true;
                                }">
                                    <template #enterButton>
                                        <a-button>
                                            <template #icon><SearchOutlined /></template>
                                        </a-button>
                                    </template>
                                </a-input-search>
                            </a-col>
                            <a-col :span="8">
                                <a-tooltip v-if="!op.id || (op.id <= 0) || (authUser.role === UserRole.ROLE_ADMIN_MASTER)" title="Chọn từ danh sách">
                                    <a-button type="primary" @click="() => {
                                        productIndexPageFinish = (recordData) => {
                                            op.product_id = recordData.id;
                                            op.product = recordData;
                                        };
                                        productIndexPageVisible = true;
                                    }">
                                        <template #icon><ShoppingCartOutlined /></template> Chọn
                                    </a-button>
                                </a-tooltip>
                            </a-col>
                        </a-row>
                    </a-form-item>
                    <a-form-item
                        label="Hình thức thanh toán"
                        :rules="{required:true,message:'Không được để trống'}"
                        :name="['order_products', pIdx, 'payment_method']"
                    >
                        <a-select v-model:value="op.payment_method">
                            <a-select-option v-for="methodCode in Object.keys(configPaymentMethod)" :key="methodCode" :value="parseInt(methodCode)">{{ configPaymentMethod[methodCode].name }}</a-select-option>
                        </a-select>
                    </a-form-item>
                    <a-form-item label="Số lượng muốn đặt ban đầu">
                        <a-input-number v-model:value="op.quantity" style="width: 100%;" :min="1" :disabled="op.product ? true : false" />
                    </a-form-item>
                    <a-card title="Xuất kho" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#680075',color:'#FFF'}" :bodyStyle="{padding:0}" size="small">
                        <template #extra>
                            <a @click="() => op.order_product_stocks.push(cloneDeep(order_product_stock_obj))">
                                <a-tooltip title="Chọn thêm hàng từ kho">
                                    <a-button type="primary" size="small">
                                        <template #icon><PlusOutlined /></template>
                                    </a-button>
                                </a-tooltip>
                            </a>
                        </template>
                            <!-- :scroll="(['xs','sm','md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300, y: '85vh' } : {}" -->
                        <a-table
                            :columns="product_stockTableColumns"
                            :data-source="op.order_product_stocks"
                            :pagination="false"
                            :row-key="ops => `ops-${ops.id || Math.random()}`"
                            size="small"
                            bordered
                            defaultExpandAllRows
                        >
                            <template #stock="{ text, record: ps, index: psIdx }">
                                <a-form-item
                                    :rules="{required:true,message:'Không được để trống'}"
                                    :name="['order_products', pIdx, 'order_product_stocks', psIdx, 'stock_id']"
                                    style="margin-bottom:0;"
                                    :help="ps.stock ? `Đang chọn: #${ps.stock_id}. ${ps.stock.name} (${ps.stock.idi}) (${number_format(ps.stock.cost_price)} ₫)` : false"
                                >
                                    <a-row :gutter="8">
                                        <a-col :span="12">
                                            <a-input-search v-model:value="ps.stock_id" readOnly @search="() => {
                                                currentStockId = ps.stock_id;
                                                stockEditPageVisible = true;
                                            }">
                                                <template #enterButton>
                                                    <a-button>
                                                        <template #icon><SearchOutlined /></template>
                                                    </a-button>
                                                </template>
                                            </a-input-search>
                                        </a-col>
                                        <a-col :span="8">
                                            <a-tooltip v-if="!ps.id || (ps.id <= 0)" title="Chọn từ danh sách">
                                                <a-button type="primary" @click="() => {
                                                    stockIndexPageFinish = (recordData) => {
                                                        ps.stock_id = recordData.id;
                                                        ps.stock = recordData;
                                                    };
                                                    stockIndexPageVisible = true
                                                }">
                                                    <template #icon><BankOutlined /></template> Chọn
                                                </a-button>
                                            </a-tooltip>
                                        </a-col>
                                    </a-row>
                                </a-form-item>
                            </template>
                            <template #status="{ record: ps, index: psIdx }">
                                <a-form-item :name="['order_products', pIdx, 'order_product_stocks', psIdx, 'status']" style="min-width: 150px;">
                                    <a-select v-model:value="ps.status" :disabled="!ps.id || (ps.id <= 0)">
                                        <a-select-option v-for="stsCode in Object.keys(configOrderProductStockStatus)" :key="stsCode" :value="parseInt(stsCode)">{{ configOrderProductStockStatus[stsCode].name }}</a-select-option>
                                    </a-select>
                                </a-form-item>
                            </template>
                            <template #amount="{ text, record: ps, index: psIdx }">
                                <a-form-item
                                    :rules="[{required:true,message:'Không được để trống'},{type:'integer'}]"
                                    :name="['order_products', pIdx, 'order_product_stocks', psIdx, 'amount']"
                                    style="margin-bottom:0;"
                                    :help="`Xem trước: ${number_format(ps.amount || 0)} ₫`"
                                >
                                    <a-input-number v-model:value="ps.amount" style="width: 100%;" :min="0" :max="2000000000" />
                                </a-form-item>
                                <div>Đã thanh toán: <a-tag :color="(ps.id ? ((ps.stock.cost_price > ps.amount) ? ps.stock.cost_price : ps.amount) : ps.amount) >= 0 ? 'green' : 'red'">{{ number_format(ps.transactions.reduce((a, b) => parseInt(a) + (parseInt(b.amount) || 0), 0)) }} ₫</a-tag></div>
                            </template>
                            <template #action="{ text, record: ps, index: psIdx }">
                                <a @click="() => ps.transactions.push({...transaction_obj})">
                                    <a-tooltip title="Thêm giao dịch">
                                        <a-button type="primary">
                                            <template #icon><PlusOutlined /></template>
                                        </a-button>
                                    </a-tooltip>
                                </a>
                                <a-popconfirm v-if="!ps.id" title="Chắc chắn muốn xóa?" @confirm="() => op.order_product_stocks.splice(psIdx, 1)">
                                    <a-button type="primary" danger>
                                        <template #icon><DeleteOutlined /></template>
                                    </a-button>
                                </a-popconfirm>
                            </template>
                            <template #expandedRowRender="{ record: ops, index: psIdx }">
                                <a-table
                                    :columns="addon_transactionsTableColumns"
                                    :data-source="ops.transactions"
                                    :pagination="false"
                                    :row-key="opst => `opst-${opst.id || Math.random()}`"
                                    size="small"
                                    bordered
                                >
                                    <template #description="{ text: value, record: pst, index: pstIdx }">
                                        <a-form-item
                                            :rules="[{required:true,message:'Không được để trống'}, {min:5,message:'Yêu cầu ghi nội dung cẩn thận (Tối thiểu 5 ký tự)'}]"
                                            :name="['order_products', pIdx, 'order_product_stocks', psIdx, 'transactions', pstIdx, 'description']" style="margin-bottom:0;"
                                        >
                                            <a-input v-model:value="pst.description" placeholder="Trả góp, trả thẳng, thanh toán sản phẩm ABC, vv..vv" type="textarea" />
                                        </a-form-item>
                                    </template>
                                    <template #amount="{ text: value, record: pst, index: pstIdx }">
                                        <a-form-item
                                            :rules="[{required:true,message:'Không được để trống'},{type:'integer'}]"
                                            :name="['order_products', pIdx, 'order_product_stocks', psIdx, 'transactions', pstIdx, 'amount']" style="margin-bottom:0;"
                                            :help="`Xem trước: ${number_format(pst.amount || 0)} ₫`"
                                        >
                                            <a-input-number v-model:value="pst.amount" style="width: 100%;" :min="-2000000000" :max="2000000000" :disabled="disabledField(pst, UserRole.ROLE_ADMIN_MASTER)" />
                                        </a-form-item>
                                    </template>
                                    <template #paid_date="{ text: value, record: pst, index: pstIdx }">
                                        <a-form-item
                                            :rules="{required:true,message:'Không được để trống'}"
                                            :name="['order_products', pIdx, 'order_product_stocks', psIdx, 'transactions', pstIdx, 'paid_date']" style="margin-bottom:0;"
                                            help="Ngày thanh toán sẽ liên kết trực tiếp tới tiền lãi/thu của tháng đó"
                                        >
                                            <a-date-picker
                                                v-model:value="pst.paid_date" format="YYYY-MM-DD HH:mm:ss" show-time type="date" :disabled="disabledField(pst)"
                                                :disabledDate="disabledLastMonthAndTomorrow"
                                            />
                                        </a-form-item>
                                    </template>
                                    <template #action="{ text: value, record: pst, index: pstIdx }">
                                        <a-popconfirm v-if="!pst.id" title="Chắc chắn muốn xóa?" @confirm="() => ops.transactions.splice(pstIdx,1)">
                                            <a-button type="primary" danger>
                                                <template #icon><DeleteOutlined /></template>
                                            </a-button>
                                        </a-popconfirm>
                                    </template>
                                </a-table>
                            </template>
                        </a-table>
                    </a-card>
                </a-card>
            </a-card>
            <a-card title="Giao dịch thêm" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#9800ab',color:'#FFF'}" :bodyStyle="{padding:0}">
                <template #extra>
                    <a @click="() => formData.transactions.push({...transaction_obj})">
                        <a-tooltip title="Thêm giao dịch">
                            <a-button type="primary">
                                <template #icon><PlusOutlined /></template>
                            </a-button>
                        </a-tooltip>
                    </a>
                </template>
                <a-table
                    :scroll="(['xs','sm','md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300, y: '85vh' } : {}"
                    :columns="addon_transactionsTableColumns"
                    :data-source="formData.transactions"
                    :pagination="false"
                    :row-key="record => `addon-tnx-${record.id || Math.random()}`"
                    size="small"
                    bordered
                >
                    <template #description="{ text, record, index }">
                        <a-form-item
                            :rules="[{required:true,message:'Không được để trống'}, {min:5,message:'Yêu cầu ghi nội dung cẩn thận (Tối thiểu 5 ký tự)'}]"
                            :name="['transactions', index, 'description']" style="margin-bottom:0;"
                        >
                            <a-input v-model:value="record.description" placeholder="Mã giảm giá, phí ship, v..v" type="textarea" />
                        </a-form-item>
                    </template>
                    <template #amount="{ text, record, index }">
                        <a-form-item
                            :rules="{required:true,message:'Không được để trống'}"
                            :name="['transactions', index, 'amount']"
                            style="margin-bottom:0;"
                            :help="`Xem trước: ${number_format(record.amount || 0)} ₫`"
                        >
                            <a-input-number v-model:value="record.amount" style="width: 100%;" :min="-2000000000" :max="2000000000" :disabled="disabledField(record, UserRole.ROLE_ADMIN_MASTER)" />
                        </a-form-item>
                    </template>
                    <template #paid_date="{ text, record, index }">
                        <a-form-item
                            :rules="{required:true,message:'Không được để trống'}"
                            :name="['transactions', index, 'paid_date']" style="margin-bottom:0;"
                            help="Ngày thanh toán sẽ liên kết trực tiếp tới tiền lãi/thu của tháng đó"
                        >
                            <a-date-picker
                                v-model:value="record.paid_date" format="YYYY-MM-DD HH:mm:ss" show-time type="date"
                                :disabled="disabledField(record)"
                                :disabledDate="disabledLastMonthAndTomorrow"
                            />
                        </a-form-item>
                    </template>
                    <template #action="{ text, record, index }">
                        <a-popconfirm v-if="!record.id" title="Chắc chắn muốn xóa?" @confirm="() => formData.transactions.splice(index,1)">
                            <a-button type="primary" danger>
                                <template #icon><DeleteOutlined /></template>
                            </a-button>
                        </a-popconfirm>
                    </template>
                </a-table>
            </a-card>
            <a-form-item :label-col="{ span: 0 }" :wrapper-col="{ span: 16, offset: (['xs','sm','md'].indexOf($grid.breakpoint) !== -1) ? 0 : 4 }">
                <a-button type="primary" htmlType="submit" block size="large">{{ id ? 'Sửa' : 'Tạo đơn' }}</a-button>
            </a-form-item>
        </a-form>
    </a-spin>

    <a-modal
        :visible="userIndexPageVisible"
        @cancel="() => userIndexPageVisible = false"
        :footer="false"
        width="98vw"
    >
        <UserIndex :onFinishSelect="(recordData) => {
            userIndexPageFinish(recordData);
            userIndexPageVisible = false;
        }" />
    </a-modal>
    <a-modal
        :visible="userEditPageVisible"
        @cancel="() => userEditPageVisible = false"
        :footer="false"
        :width="800"
    >
        <UserEdit :userId="currentUserId" />
    </a-modal>

    <a-modal
        :visible="stockIndexPageVisible"
        @cancel="() => stockIndexPageVisible = false"
        :footer="false"
        width="98vw"
    >
        <StockIndex :onFinishSelect="(recordData) => {
            stockIndexPageFinish(recordData);
            stockIndexPageVisible = false;
        }" />
    </a-modal>
    <a-modal
        :visible="stockEditPageVisible"
        @cancel="() => stockEditPageVisible = false"
        :footer="false"
        width="98vw"
    >
        <StockEdit :stockId="currentStockId" />
    </a-modal>

    <a-modal
        :visible="productIndexPageVisible"
        @cancel="() => productIndexPageVisible = false"
        :footer="false"
        width="98vw"
    >
        <ProductIndex :onFinishSelect="(recordData) => {
            productIndexPageFinish(recordData);
            productIndexPageVisible = false;
        }" />
    </a-modal>
    <a-modal
        :visible="productEditPageVisible"
        @cancel="() => productEditPageVisible = false"
        :footer="false"
        width="98vw"
    >
        <ProductEdit :productId="currentProductId" />
    </a-modal>
</template>

<script>
import { reactive, ref } from 'vue';
import moment from 'moment';

import {
    PrinterOutlined, BankOutlined, ShoppingCartOutlined,
    PlusOutlined, ReloadOutlined, DeleteOutlined,
    UserOutlined, SearchOutlined,
} from '@ant-design/icons-vue';

import { number_format, cloneDeep, date_format, defineAsyncComponent, } from '~/helpers';
import UserRole from '~/dashboard/configs/UserRole';
import OrderStatus, { Config as configOrderStatus } from '~/configs/OrderStatus';
import OrderProductStockStatus, { Config as configOrderProductStockStatus } from '~/configs/OrderProductStockStatus';
import PaymentMethod, { Config as configPaymentMethod } from '~/configs/PaymentMethod';
import RequestRepository from '~/dashboard/utils/RequestRepository';
import User from '~/dashboard/utils/User';

const addon_transactionsTableColumns = [
    {
        title: '#ID',
        dataIndex: 'id',
    },
    {
        title: '* Nội dung',
        dataIndex: 'description',
        slots: {
            customRender: 'description',
        },
    },
    {
        title: '* Số tiền',
        dataIndex: 'amount',
        slots: {
            customRender: 'amount',
        },
    },
    {
        title: '* Ngày thanh toán',
        dataIndex: 'paid_date',
        slots: {
            customRender: 'paid_date',
        },
    },
    {
        title: 'Hành động',
        key: 'action',
        slots: {
            customRender: 'action',
        },
    },
];

const product_stockTableColumns = [
    {
        title: '#ID',
        dataIndex: 'id',
    },
    {
        title: '* Hàng trong kho',
        dataIndex: 'stock',
        slots: {
            customRender: 'stock',
        },
    },
    {
        title: '* Trạng thái',
        dataIndex: 'status',
        slots: {
            customRender: 'status',
        },
    },
    {
        title: '* Tổng tiền phải thanh toán',
        dataIndex: 'amount',
        slots: {
            customRender: 'amount',
        },
    },
    {
        title: 'Hành động',
        key: 'action',
        slots: {
            customRender: 'action',
        },
    },
];

export default {
    props: {
        orderId: Number,
    },

    emits: [
        'orderUpdated'
    ],

    components: {
        UserIndex: defineAsyncComponent(() => import('~/dashboard/pages/users/Index.vue')),
        UserEdit: defineAsyncComponent(() => import('~/dashboard/pages/users/Edit.vue')),
        ProductIndex: defineAsyncComponent(() => import('~/dashboard/pages/products/Index.vue')),
        ProductEdit: defineAsyncComponent(() => import('~/dashboard/pages/products/Edit.vue')),
        StockIndex: defineAsyncComponent(() => import('~/dashboard/pages/stocks/Index.vue')),
        StockEdit: defineAsyncComponent(() => import('~/dashboard/pages/stocks/Edit.vue')),

        PrinterOutlined, BankOutlined, ShoppingCartOutlined,
        PlusOutlined, ReloadOutlined, DeleteOutlined,
        UserOutlined, SearchOutlined,
    },

    setup() {
        const ruleForm = ref();
        const formData = reactive({
            id: undefined,
            deal_date: moment(),
            note: undefined,
            customer_id: undefined,
            order_products: [],
            status: OrderStatus.STS_PROCESSING,
            transactions: [],
        });

        return {
            ruleForm,
            formData,
        };
    },

    data() {
        return {
            userIndexPageVisible: false,
            userIndexPageFinish: null,
            userEditPageVisible: false,
            currentUserId: undefined,

            productIndexPageVisible: false,
            productIndexPageFinish: null,
            productEditPageVisible: false,
            currentProductId: undefined,

            stockIndexPageVisible: false,
            stockIndexPageFinish: null,
            stockEditPageVisible: false,
            currentStockId: undefined,

            orderInfo: {},
            categories: [],
            productData: [],
            stockData: [],

            addon_transactionsTableColumns,
            product_stockTableColumns,

            orderInfoLoading: false,
            rules: {
                status: {required: true},
                note: {required: true},
                customer_id: {required: true},
            },

            transaction_obj: {
                id: undefined,
                description: undefined,
                amount: undefined,
                paid_date: undefined,
            },

            order_product_obj: {
                id: undefined,
                order_id: undefined,
                product_id: null,
                payment_method: PaymentMethod.PM_ONCE,
                order_product_stocks: [],
                quantity: undefined,
            },
            order_product_stock_obj: {
                id: undefined,
                order_product_id: undefined,
                stock_id: null,
                status: OrderProductStockStatus.STS_SOLD,
                amount: undefined,
                transactions: [],
            },


            OrderStatus,
            configOrderStatus,
            OrderProductStockStatus,
            configOrderProductStockStatus,
            PaymentMethod,
            configPaymentMethod,
            UserRole,

            authUser: User.info(),
        }
    },
    mounted() {
        this.loadCategoriesTree();

        if (this.id) {
            this.loadOrder(this.id);
        }
    },
    watch: {
        orderId() {
            this.formData.id = undefined;
        },

        id(to) {
            if (to) {
                this.loadOrder(to);
            } else {
                this.$refs.ruleForm.resetFields();

                this.orderInfo = {};

                this.formData.order_products = [];

                this.formData.transactions = [];

                this.formData.deal_date = moment();

                this.orderInfoLoading = false;
            }
        },
    },
    computed: {
        id() {
            return this.orderId || this.formData.id;
        },
    },
    methods: {
        cloneDeep,
        number_format,
        date_format,

        disabledLastMonthAndTomorrow(current) {
            return !this.authUser.hasPermission(UserRole.ROLE_ADMIN_SUB_MASTER) &&
                current && ((current < moment().startOf('month')) || (current > moment().endOf('day')));
        },
        disabledField(record, needRole = UserRole.ROLE_ADMIN_SUB_MASTER) {
            return !this.authUser.hasPermission(needRole) && record.id && (record.id >= 0);
        },

        loadCategoriesTree(){
            RequestRepository.get('/categories')
                .then(res => {
                    this.categories = res.data.data.sort((a, b) => a.parent_id - b.parent_id);

                    const len = this.categories.length;
                    for (let i = 0; i < len; i++) {
                        const elm = this.categories[i];
                        if (elm.parent_id !== 0) {
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
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    // this.categoriesTreeLoading = false;
                });
        },

        loadOrder(id) {
            this.orderInfoLoading = true;

            // Reset popup data
            this.currentUserId = undefined;
            this.currentProductId = undefined;
            this.currentStockId = undefined;

            RequestRepository.get(`/orders/${id}`)
                .then(res => {
                    const orderData = res.data.data;
                    if (!orderData.id) {
                        throw res;
                    }

                    _.assign(this.formData, _.pick(orderData, _.keys(this.formData)));
                    const deal_date = moment(orderData.deal_date);
                    this.formData.deal_date = deal_date.isValid() ? deal_date : undefined;
                    this.formData.transactions = this.formData.transactions.map(value => {
                        return {
                            ...value,
                            paid_date: moment(value.paid_date),
                        }
                    });
                    this.formData.order_products = this.formData.order_products.map(op_value => {
                        return {
                            ...op_value,
                            order_product_stocks: op_value.order_product_stocks.map(ops_value => {
                                return {
                                    ...ops_value,
                                    transactions: ops_value.transactions.map(opst_value => {
                                        return {
                                            ...opst_value,
                                            paid_date: moment(opst_value.paid_date)
                                        };
                                    }),
                                };
                            }),
                        };
                    });

                    this.orderInfo = cloneDeep(orderData);

                    // Load tree

                    this.orderInfoLoading = false;
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                });
        },

        onFinish() {
            this.orderInfoLoading = true;

            const orderId = this.id;
            const request = orderId ? RequestRepository.put : RequestRepository.post;
            request('/orders' + (orderId ? `/${orderId}` : ''), {
                ...this.formData,
                deal_date: moment(this.formData.deal_date).format("YYYY-MM-DD HH:mm:ss"),
                transactions: this.formData.transactions.map(value => {
                    return {
                        ...value,
                        paid_date: moment(value.paid_date).format("YYYY-MM-DD HH:mm:ss"),
                    };
                }),
                order_products: this.formData.order_products.map(op_value => {
                    return {
                        ...op_value,
                        product: undefined,
                        order_product_stocks: op_value.order_product_stocks.map(ops_value => {
                            return {
                                ...ops_value,
                                stock: undefined,
                                transactions: ops_value.transactions.map(opst_value => {
                                    return {
                                        ...opst_value,
                                        paid_date: moment(opst_value.paid_date).format('YYYY-MM-DD HH:mm:ss')
                                    };
                                }),
                            };
                        }),
                    };
                }),
            })
                .then(res => {
                    const orderData = res.data.data;

                    if (!orderData.id) {
                        throw res;
                    }

                    this.$message.success(orderId ? 'Đã sửa sản phẩm thành công' : 'Đã thêm sản phẩm thành công');

                    this.$emit('orderUpdated', orderId);

                    this.loadOrder(orderData.id);
                })
                .catch(err => {
                    this.orderInfoLoading = false;

                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                });
        },

        onFinishSelectUser(recordData) {
            this.formData.customer_id = recordData.id;

            this.userIndexPageVisible = false;
        },

        onFinishSelectProduct(recordData) {
            this.order_product.product_id = recordData.id;

            this.productIndexPageVisible = false;
        },

        onFinishSelectStock(recordData) {
            this.order_product_stock.stock_id = recordData.id;

            this.stockIndexPageVisible = false;
        },

        printOrder() {
            // Fixes dual-screen position                             Most browsers      Firefox
            const dualScreenLeft = window.screenLeft !==  undefined ? window.screenLeft : window.screenX;
            const dualScreenTop = window.screenTop !==  undefined   ? window.screenTop  : window.screenY;

            const width = window.innerWidth ? window.innerWidth : document.documentElement.clientWidth ? document.documentElement.clientWidth : screen.width;
            const height = window.innerHeight ? window.innerHeight : document.documentElement.clientHeight ? document.documentElement.clientHeight : screen.height;

            const systemZoom = width / window.screen.availWidth;
            const left = (width - w) / 2 / systemZoom + dualScreenLeft
            const top = (height - h) / 2 / systemZoom + dualScreenTop

            const w = width;
            const h = height;

            const newWindow = window.open('', '', `
                scrollbars=yes,
                width=${w / systemZoom},
                height=${h / systemZoom},
                top=${top},
                left=${left}
            `);

            const newDoc = newWindow.document;

            newDoc.write('<html><body>');

            newDoc.write(`Hoá đơn #${this.id} (${configOrderStatus[this.formData.status].name})`);
            newDoc.write('<table><thead><tr>');
            newDoc.write('<th>STT</th>');
            newDoc.write('<th>STT</th>');
            newDoc.write('<th>STT</th>');
            newDoc.write('</tr></thead>')
            newDoc.write('<tbody>');
            newDoc.write('</tbody></table>');

            newDoc.write('</body></html>');

            // window.onclose = newWindow.close;
            newWindow.focus();
            newWindow.print();
            newWindow.close();
        }
    },
}
</script>
