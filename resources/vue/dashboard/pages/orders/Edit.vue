<template>
    <div>
        <a-spin :spinning="orderInfoLoading">
            <a-page-header
                :title="id ? 'Chỉnh sửa hóa đơn' : 'Tạo hóa đơn'"
                :sub-title="id ? `#${id}` : false"
            >
                <template slot="extra">
                    <a-tooltip title="In hoá đơn" v-if="id">
                        <a-button type="primary" icon="printer" @click="() => printOrder()" style="float:right;" />
                    </a-tooltip>
                </template>
            </a-page-header>
            <a-form-model
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? { span: 4 } : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? { span: 14 } : {}"
            >
                <a-form-model-item label="Trạng thái" prop="status">
                    <a-select v-model="formData.status">
                        <a-select-option v-for="stsCode in Object.keys(configOrderStatus)" :key="stsCode" :value="parseInt(stsCode)">{{ configOrderStatus[stsCode].name }}</a-select-option>
                    </a-select>
                </a-form-model-item>
                <a-form-model-item
                    label="Ngày xuất đơn"
                    prop="deal_date"
                    help="Ngày xuât đơn cho khách hàng"
                    :rules="{required:true}"
                >
                    <a-date-picker
                        v-model="formData.deal_date"
                        format="YYYY-MM-DD HH:mm:ss"
                        show-time
                        type="date"
                    />
                </a-form-model-item>
                <a-form-model-item
                    label="Khách hàng" ref="customer_id" prop="customer_id"
                    :help="orderInfo.customer ? `Đang chọn: #${orderInfo.customer_id}. ${orderInfo.customer.name} (Phone: ${orderInfo.customer.phone || 'Chưa có'})` : false"
                >
                    <a-row :gutter="8">
                        <a-col :span="12">
                            <a-input-search v-model="formData.customer_id" readOnly @search="() => userEditPageVisible = true">
                                <a-button icon="search" slot="enterButton" />
                            </a-input-search>
                        </a-col>
                        <a-col :span="8">
                            <a-tooltip title="Chọn từ danh sách">
                                <a-button type="primary" icon="user" @click="() => userIndexPageVisible = true">Chọn</a-button>
                            </a-tooltip>
                        </a-col>
                    </a-row>
                </a-form-model-item>
                <a-form-model-item label="Ghi chú" prop="note">
                    <a-textarea v-model="formData.note" placeholder="Tên khách hàng, loại thanh toán, chi phí sinh hoạt, lương nhân viên, v..v" />
                </a-form-model-item>
                <a-card title="Sản phẩm đặt mua" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#9800ab',color:'#FFF'}">
                    <a slot="extra" @click="() => {formData.order_products.push(Object.assign({}, order_product_obj));}">
                        <a-tooltip title="Thêm sản phẩm">
                            <a-button type="primary" icon="plus" />
                        </a-tooltip>
                    </a>
                    <a-card :title="`Sản phẩm #${pIdx}`" v-for="(p, pIdx) in formData.order_products" :key="p.id" style="margin-bottom: 16px;" :headStyle="{backgroundColor:'#f18e1f',color:'#FFF'}">
                        <a-popconfirm v-if="!p.id" slot="extra" title="Chắc chắn muốn xóa?" @confirm="() => formData.order_products.splice(pIdx, 1)">
                            <a-button type="danger" icon="delete"></a-button>
                        </a-popconfirm>
                        <a-form-model-item
                            label="Sản phẩm"
                            :rules="{required:true,message:'Không được để trống'}"
                            :prop="'order_products.'+pIdx+'.product_id'"
                            :help="(p.id && p.product) ? `Đang chọn: #${p.product.id}. ${p.product.name} (${number_format(p.product.price)} VND)` : false"
                        >
                            <a-row :gutter="8">
                                <a-col :span="12">
                                    <a-input-search v-model="p.product_id" readOnly @search="() => { order_product = p;productEditPageVisible = true; }">
                                        <a-button icon="search" slot="enterButton" />
                                    </a-input-search>
                                </a-col>
                                <a-col :span="8">
                                    <a-tooltip v-if="!p.id || (p.id <= 0) || (authUser.role === UserRole.ROLE_ADMIN_MASTER)" title="Chọn từ danh sách">
                                        <a-button type="primary" icon="shopping-cart" @click="() => {order_product = p;productIndexPageVisible = true}">Chọn</a-button>
                                    </a-tooltip>
                                </a-col>
                            </a-row>
                        </a-form-model-item>
                        <a-form-model-item
                            label="Hình thức thanh toán"
                            :rules="{required:true,message:'Không được để trống'}"
                            :prop="'order_products.'+pIdx+'.payment_method'"
                        >
                            <a-select v-model="p.payment_method">
                                <a-select-option v-for="methodCode in Object.keys(configPaymentMethod)" :key="methodCode" :value="parseInt(methodCode)">{{ configPaymentMethod[methodCode].name }}</a-select-option>
                            </a-select>
                        </a-form-model-item>
                        <a-form-model-item label="Số lượng muốn đặt ban đầu">
                            <a-input-number v-model="p.quantity" style="width: 100%;" :min="1" :disabled="p.product ? true : false" />
                        </a-form-model-item>
                        <a-card title="Xuất kho" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#680075',color:'#FFF'}" :bodyStyle="{padding:0}">
                            <a slot="extra" @click="() => p.order_product_stocks.push(Object.assign({}, order_product_stock_obj))">
                                <a-tooltip title="Chọn thêm hàng từ kho">
                                    <a-button type="primary" icon="plus" />
                                </a-tooltip>
                            </a>
                            <a-table
                                :columns="product_stockTableColumns"
                                :data-source="p.order_product_stocks"
                                :pagination="false"
                                :row-key="record => `po-${record.id || Math.random()}`"
                                size="small"
                                bordered
                            >
                                <template slot="stock" slot-scope="text, ps, psIdx">
                                    <a-form-model-item
                                        :rules="{required:true,message:'Không được để trống'}"
                                        :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.stock_id'"
                                        style="margin-bottom:0;"
                                        :help="(ps.id && ps.stock) ? `Đang chọn: #${ps.stock_id}. ${ps.stock.name} (${ps.stock.idi}) (${number_format(ps.stock.cost_price)} VND)` : false"
                                    >
                                        <a-row :gutter="8">
                                            <a-col :span="12">
                                                <a-input-search v-model="ps.stock_id" readOnly @search="() => { order_product_stock = ps;stockEditPageVisible = true; }">
                                                    <a-button icon="search" slot="enterButton" />
                                                </a-input-search>
                                            </a-col>
                                            <a-col :span="8">
                                                <a-tooltip v-if="!ps.id || (ps.id <= 0)" title="Chọn từ danh sách">
                                                    <a-button type="primary" icon="bank" @click="() => {order_product_stock = ps;stockIndexPageVisible = true}">Chọn</a-button>
                                                </a-tooltip>
                                            </a-col>
                                        </a-row>
                                    </a-form-model-item>
                                </template>
                                <template slot="status" slot-scope="value, ps, psIdx">
                                    <a-form-model-item :prop="`order_products.${pIdx}.order_product_stocks.${psIdx}.status`" style="min-width: 150px;">
                                        <a-select v-model="ps.status" :disabled="!ps.id || (ps.id <= 0)">
                                            <a-select-option v-for="stsCode in Object.keys(configOrderProductStockStatus)" :key="stsCode" :value="parseInt(stsCode)">{{ configOrderProductStockStatus[stsCode].name }}</a-select-option>
                                        </a-select>
                                    </a-form-model-item>
                                </template>
                                <template slot="amount" slot-scope="text, ps, psIdx">
                                    <a-form-model-item
                                        :rules="{required:true,message:'Không được để trống'}"
                                        :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.amount'"
                                        style="margin-bottom:0;"
                                        :help="`VND: ${number_format(ps.amount || 0)}`"
                                    >
                                        <a-input-number v-model="ps.amount" style="width: 100%;" :min="0" :max="2000000000" />
                                    </a-form-model-item>
                                    <div>Đã thanh toán: <PaidAmount :needAmount="ps.id ? ((ps.stock.cost_price > ps.amount) ? ps.stock.cost_price : ps.amount) : ps.amount" :amount="ps.transactions.reduce((a, b) => parseInt(a) + (parseInt(b.amount) || 0), 0)" /></div>
                                </template>
                                <template slot="action" slot-scope="text, ps, psIdx">
                                    <a @click="() => ps.transactions.push(Object.assign({}, transaction_obj))">
                                        <a-tooltip title="Thêm giao dịch">
                                            <a-button type="primary" icon="plus" />
                                        </a-tooltip>
                                    </a>
                                    <a-popconfirm v-if="!ps.id" title="Chắc chắn muốn xóa?" @confirm="() => p.order_product_stocks.splice(psIdx, 1)">
                                        <a-button type="danger" icon="delete" />
                                    </a-popconfirm>
                                </template>
                                <a-table
                                    slot="expandedRowRender"
                                    slot-scope="ps, psIdx"
                                    :columns="addon_transactionsTableColumns"
                                    :data-source="ps.transactions"
                                    :pagination="false"
                                    :row-key="ps => `p-addon-tnx-${ps.id || Math.random()}`"
                                    size="small"
                                    bordered
                                >
                                    <template slot="description" slot-scope="value, pst, pstIdx">
                                        <a-form-model-item
                                            :rules="[{required:true,message:'Không được để trống'}, {min:10,message:'Yêu cầu ghi nội dung cẩn thận (Tối thiểu 10 ký tự)'}]"
                                            :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.transactions.'+pstIdx+'.description'" style="margin-bottom:0;"
                                        >
                                            <a-input v-model="pst.description" placeholder="Trả góp, trả thẳng, thanh toán sản phẩm ABC, vv..vv" type="textarea" />
                                        </a-form-model-item>
                                    </template>
                                    <template slot="amount" slot-scope="value, pst, pstIdx">
                                        <a-form-model-item
                                            :rules="{required:true,message:'Không được để trống'}"
                                            :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.transactions.'+pstIdx+'.amount'" style="margin-bottom:0;"
                                            :help="`VND: ${number_format(pst.amount || 0)}`"
                                        >
                                            <a-input-number v-model="pst.amount" style="width: 100%;" :min="-2000000000" :max="2000000000" :disabled="pst.id && (pst.id >= 0)" />
                                        </a-form-model-item>
                                    </template>
                                    <template slot="paid_date" slot-scope="value, pst, pstIdx">
                                        <a-form-model-item
                                            :rules="{required:true,message:'Không được để trống'}"
                                            :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.transactions.'+pstIdx+'.paid_date'" style="margin-bottom:0;"
                                            help="Ngày thanh toán sẽ liên kết trực tiếp tới tiền lãi/thu của tháng đó"
                                        >
                                            <a-date-picker v-model="pst.paid_date" format="YYYY-MM-DD HH:mm:ss" show-time type="date" :disabled="pst.id && (pst.id >= 0)" />
                                        </a-form-model-item>
                                    </template>
                                    <template slot="action" slot-scope="value, pst, pstIdx">
                                        <a-popconfirm v-if="!pst.id" title="Chắc chắn muốn xóa?" @confirm="() => ps.transactions.splice(pstIdx,1)">
                                            <a-button type="danger" icon="delete" />
                                        </a-popconfirm>
                                    </template>
                                </a-table>
                            </a-table>
                        </a-card>
                    </a-card>
                </a-card>
                <a-card title="Giao dịch thêm" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#9800ab',color:'#FFF'}" :bodyStyle="{padding:0}">
                    <a slot="extra" @click="() => formData.transactions.push(Object.assign({}, transaction_obj))">
                        <a-tooltip title="Thêm giao dịch">
                            <a-button type="primary" icon="plus" />
                        </a-tooltip>
                    </a>
                    <a-table
                        :columns="addon_transactionsTableColumns"
                        :data-source="formData.transactions"
                        :pagination="false"
                        :row-key="record => `addon-tnx-${record.id || Math.random()}`"
                        size="small"
                        bordered
                    >
                        <template slot="description" slot-scope="text, record, index">
                            <a-form-model-item
                                :rules="[{required:true,message:'Không được để trống'}, {min:10,message:'Yêu cầu ghi nội dung cẩn thận (Tối thiểu 10 ký tự)'}]"
                                :prop="`transactions.${index}.description`" style="margin-bottom:0;"
                            >
                                <a-input v-model="record.description" placeholder="Mã giảm giá, phí ship, v..v" type="textarea" />
                            </a-form-model-item>
                        </template>
                        <template slot="amount" slot-scope="text, record, index">
                            <a-form-model-item
                                :rules="{required:true,message:'Không được để trống'}"
                                :prop="`transactions.${index}.amount`"
                                style="margin-bottom:0;"
                                :help="`VND: ${number_format(record.amount || 0)}`"
                            >
                                <a-input-number v-model="record.amount" style="width: 100%;" :min="-2000000000" :max="2000000000" :disabled="record.id && (record.id >= 0)" />
                            </a-form-model-item>
                        </template>
                        <template slot="paid_date" slot-scope="text, record, index">
                            <a-form-model-item
                                :rules="{required:true,message:'Không được để trống'}"
                                :prop="'transactions.'+index+'.paid_date'" style="margin-bottom:0;"
                                help="Ngày thanh toán sẽ liên kết trực tiếp tới tiền lãi/thu của tháng đó"
                            >
                                <a-date-picker v-model="record.paid_date" format="YYYY-MM-DD HH:mm:ss" show-time type="date" :disabled="record.id && (record.id >= 0)" />
                            </a-form-model-item>
                        </template>
                        <template slot="action" slot-scope="text, record, index">
                            <a-popconfirm v-if="!record.id" title="Chắc chắn muốn xóa?" @confirm="() => formData.transactions.splice(index,1)">
                                <a-button type="danger" icon="delete" />
                            </a-popconfirm>
                        </template>
                    </a-table>
                </a-card>
                <a-form-model-item :label-col="{ span: 0 }" :wrapper-col="{ span: 24 }">
                    <a-button
                        type="primary" htmlType="submit" @click="() => $refs.ruleForm.validate((valid) => { if (valid) onFinish() })"
                        block
                        size="large"
                    >{{ id ? 'Sửa' : 'Tạo đơn' }}</a-button>
                </a-form-model-item>
            </a-form-model>
        </a-spin>

        <a-modal
            :visible="userIndexPageVisible"
            @cancel="() => userIndexPageVisible = false"
            :footer="false"
            :width="1000"
        >
            <UserIndex :onFinishSelect="onFinishSelectUser" />
        </a-modal>
        <a-modal
            :visible="userEditPageVisible"
            @cancel="() => userEditPageVisible = false"
            :footer="false"
            :width="800"
        >
            <UserEdit :userId="formData.customer_id" />
        </a-modal>

        <a-modal
            :visible="stockIndexPageVisible"
            @cancel="() => stockIndexPageVisible = false"
            :footer="false"
            width="95vw"
        >
            <StockIndex :onFinishSelect="onFinishSelectStock" />
        </a-modal>
        <a-modal
            :visible="stockEditPageVisible"
            @cancel="() => stockEditPageVisible = false"
            :footer="false"
            width="95vw"
        >
            <StockEdit :stockId="order_product_stock.stock_id" />
        </a-modal>

        <a-modal
            :visible="productIndexPageVisible"
            @cancel="() => productIndexPageVisible = false"
            :footer="false"
            width="95vw"
        >
            <ProductIndex :onFinishSelect="onFinishSelectProduct" />
        </a-modal>
        <a-modal
            :visible="productEditPageVisible"
            @cancel="() => productEditPageVisible = false"
            :footer="false"
            width="95vw"
        >
            <ProductEdit :productId="order_product.product_id" />
        </a-modal>
    </div>
</template>

<script>
import moment from 'moment';

import UserRole from '../../configs/UserRole';
import OrderStatus, { Config as configOrderStatus } from '../../configs/OrderStatus';
import OrderProductStockStatus, { Config as configOrderProductStockStatus } from '../../configs/OrderProductStockStatus';
import PaymentMethod, { Config as configPaymentMethod } from '../../configs/PaymentMethod';

import { number_format } from '../../../helpers';
import User from '../../utils/User';

const PaidAmount = {
    props: {
        amount: Number,
        needAmount: Number,
    },

    render: function(createElement) {
        return createElement('a-tag', {
            attrs: {
                color: this.amount >= this.needAmount ? 'green' : 'red',
            },

            domProps: {
                innerText: number_format(this.amount),
            }
        });
    },
};

const addon_transactionsTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
    },
    {
        title: '* Nội dung',
        dataIndex: 'description',
        scopedSlots: {
            customRender: 'description',
        },
    },
    {
        title: '* Số tiền',
        dataIndex: 'amount',
        scopedSlots: {
            customRender: 'amount',
        },
    },
    {
        title: '* Ngày thanh toán',
        dataIndex: 'paid_date',
        scopedSlots: {
            customRender: 'paid_date',
        },
    },
    {
        title: 'Hành động',
        key: 'action',
        scopedSlots: {
            customRender: 'action',
        },
    },
];

const product_stockTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
    },
    {
        title: '* Hàng trong kho',
        dataIndex: 'stock',
        scopedSlots: {
            customRender: 'stock',
        },
    },
    {
        title: '* Trạng thái',
        dataIndex: 'status',
        scopedSlots: {
            customRender: 'status',
        },
    },
    {
        title: '* Tổng tiền phải thanh toán',
        dataIndex: 'amount',
        scopedSlots: {
            customRender: 'amount',
        },
    },
    {
        title: 'Hành động',
        key: 'action',
        scopedSlots: {
            customRender: 'action',
        },
    },
];

export default {
    props: {
        orderId: Number,
    },
    components: {
        PaidAmount,
        UserIndex: () => import('../users/Index'),
        UserEdit: () => import('../users/Edit'),
        ProductIndex: () => import('../products/Index'),
        ProductEdit: () => import('../products/Edit'),
        StockIndex: () => import('../stocks/Index'),
        StockEdit: () => import('../stocks/Edit'),
    },
    data() {
        return {
            userIndexPageVisible: false,
            userEditPageVisible: false,

            productIndexPageVisible: false,
            productEditPageVisible: false,
            order_product: {},

            stockIndexPageVisible: false,
            stockEditPageVisible: false,
            order_product_stock: {},

            orderInfo: {},
            categories: [],
            productData: [],
            stockData: [],

            addon_transactionsTableColumns,
            product_stockTableColumns,

            orderInfoLoading: false,
            formData: {
                deal_date: moment(),
                note: undefined,
                customer_id: undefined,
                order_products: [],
                status: OrderStatus.STS_PROCESSING,
                transactions: [],
            },
            rules: {
                status: {required: true,},
                note: {required: true,},
                customer_id: {required: true},
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
            this.loadOrder(this.id)
        }
    },
    watch: {
        id(to) {
            if (to) {
                this.loadOrder(to);
            } else {
                this.$refs.ruleForm.resetFields();

                this.formData.order_products = [];

                this.formData.transactions = [];
            }
        },
    },
    computed: {
        id() {
            return this.orderId || this.$route.params.id;
        },
        transaction_obj() {
            return {
                id: undefined,
                description: undefined,
                amount: undefined,
                paid_date: undefined,
            }
        },
        order_product_obj() {
            return {
                id: undefined,
                order_id: undefined,
                product_id: null,
                payment_method: PaymentMethod.PM_ONCE,
                order_product_stocks: [],
                quantity: undefined,
            }
        },
        order_product_stock_obj() {
            return {
                id: undefined,
                order_product_id: undefined,
                stock_id: null,
                status: OrderProductStockStatus.STS_SOLD,
                amount: undefined,
                transactions: [],
            }
        },

    },
    methods: {
        number_format,
        loadCategoriesTree(){
            axios.get('/api/categories')
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
                    if (err.response && err.response.data.message) {
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
            axios.get(`/api/orders/${id}`)
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

                    this.orderInfo = orderData;

                    // Load tree

                    this.orderInfoLoading = false;
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
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
            axios({
                url: '/api/orders' + (orderId ? `/${orderId}` : ''),
                method: orderId ? 'put' : 'post',
                data: {
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
                            order_product_stocks: op_value.order_product_stocks.map(ops_value => {
                                return {
                                    ...ops_value,
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
                }
            })
                .then(res => {
                    const orderData = res.data.data;

                    if (!orderData.id) {
                        throw res;
                    }

                    if (orderId) {
                        this.$message.success('Đã sửa sản phẩm thành công');

                        this.loadOrder(orderData.id);
                    } else {
                        this.$message.success('Đã thêm sản phẩm thành công');

                        this.orderInfoLoading = false;

                        this.$router.push({ path: `/orders/${orderData.id}/edit` });
                    }
                })
                .catch(err => {
                    this.orderInfoLoading = false;

                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
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
