<template>
    <div>
        <a-spin :spinning="orderInfoLoading">
            <h2>{{ $route.params.id ? `Chỉnh sửa hóa đơn #${$route.params.id}` : 'Tạo hóa đơn đặt hàng' }}</h2>
            <a-form-model
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? labelCol : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? wrapperCol : {}"
            >
                <a-form-model-item label="Trạng thái" prop="status">
                    <a-select v-model="formData.status">
                        <a-select-option v-for="stsCode in Object.keys(configOrderStatus)" :key="stsCode" :value="parseInt(stsCode)">{{ configOrderStatus[stsCode].name }}</a-select-option>
                    </a-select>
                </a-form-model-item>
                <a-form-model-item label="Khách hàng" ref="customer_id" prop="customer_id">
                    <a-input v-model="formData.customer_id" />
                </a-form-model-item>
                <a-form-model-item label="Ghi chú">
                    <a-textarea v-model="formData.note" />
                </a-form-model-item>
                <a-card title="Sản phẩm đặt mua" style="margin-bottom:16px;" :headStyle="{backgroundColor:'#9800ab',color:'#FFF'}">
                    <a slot="extra" @click="() => {formData.order_products.push(Object.assign({}, order_product_obj));}">
                        <a-tooltip title="Thêm sản phẩm">
                            <a-button type="primary" icon="plus" />
                        </a-tooltip>
                    </a>
                    <a-card v-for="(p, pIdx) in formData.order_products" :key="p.id" style="margin-bottom: 16px;" :headStyle="{backgroundColor:'#f18e1f',color:'#FFF'}">
                        <template slot="title">
                            <template v-if="p.id && p.product">
                                <a-tooltip title="Xem">
                                    <RouterLink :to="'/products/'+p.product.id+'/edit'">Đang chọn: #{{ p.product.id + '. ' + p.product.name + ' ('+ number_format(p.product.price) +' VND)' }}</RouterLink>
                                </a-tooltip>
                            </template>
                            <template v-else>{{ 'Sản phẩm #'+pIdx }}</template>
                        </template>
                        <a-popconfirm slot="extra" title="Chắc chắn muốn xóa?" @confirm="() => formData.order_products.splice(pIdx, 1)">
                            <a-button type="primary" icon="delete"></a-button>
                        </a-popconfirm>
                        <a-form-model-item label="Sản phẩm" :rules="{required:true}" :prop="'order_products.'+pIdx+'.product_id'">
                            <a-tree-select show-search treeNodeFilterProp="title" :tree-data="productData" :load-data="loadCategoryProducts" placeholder="Please select" v-model="p.product_id" />
                        </a-form-model-item>
                        <a-form-model-item label="Hình thức thanh toán" :rules="{required:true}">
                            <a-select v-model="p.payment_method">
                                <a-select-option :value="1">
                                    Trả thẳng 100%
                                </a-select-option>
                                <a-select-option :value="2">
                                    Trả góp
                                </a-select-option>
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
                                :row-key="record => record.id"
                                bordered
                            >
                                <template slot="stock" slot-scope="text, ps, psIdx">
                                    <a-tooltip v-if="ps.id" title="Xem">
                                        <RouterLink :to="'/stocks/'+ps.stock.id+'/edit'">Đang chọn: #{{ ps.stock.id + '. ' + ps.stock.name + ' ('+ ps.stock.idi +')' + ' ('+ (number_format(ps.stock.cost_price)) +' VND)' }}</RouterLink>
                                    </a-tooltip>
                                    <template v-else>{{ 'Hàng trong kho #'+psIdx }}</template>
                                    <a-form-model-item :rules="{required:true}" :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.stock_id'" style="margin-bottom:0;">
                                        <a-tree-select show-search treeNodeFilterProp="title" :tree-data="stockData" :load-data="loadCategoryStocks" placeholder="Please select" v-model="ps.stock_id" />
                                    </a-form-model-item>
                                </template>
                                <template
                                    slot="amount"
                                    slot-scope="text, ps, psIdx"
                                >
                                    <a-form-model-item label="" :rules="{required:true}" :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.amount'" style="margin-bottom:0;"
                                        :help="`VND: ${number_format(ps.amount || 0)}`"
                                    >
                                        <a-input-number v-model="ps.amount" style="width: 100%;" :min="0" :max="2000000000" />
                                    </a-form-model-item>
                                    <div>Đã thanh toán: <PaidAmount :needAmount="ps.id ? ((ps.stock.cost_price > ps.amount) ? ps.stock.cost_price : ps.amount) : ps.amount" :amount="ps.transactions.reduce((a, b) => a + (b.amount || 0), 0)" /></div>
                                </template>
                                <template slot="action" slot-scope="text, ps, psIdx">
                                    <a @click="() => ps.transactions.push(Object.assign({}, transaction_obj))">
                                        <a-tooltip title="Thêm giao dịch">
                                            <a-button type="primary" icon="plus"></a-button>
                                        </a-tooltip>
                                    </a>
                                    <a-popconfirm title="Chắc chắn muốn xóa?" @confirm="() => p.order_product_stocks.splice(psIdx, 1)">
                                        <a-button type="primary" icon="delete"></a-button>
                                    </a-popconfirm>
                                </template>
                                <a-table
                                    slot="expandedRowRender"
                                    slot-scope="ps, psIdx"
                                    :columns="addon_transactionsTableColumns"
                                    :data-source="ps.transactions"
                                    :pagination="false"
                                    :row-key="ps => ps.id"
                                    bordered
                                >
                                    <template slot="description" slot-scope="text, pst, pstIdx">
                                        <a-form-model-item :rules="{required:true}" :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.transactions.'+pstIdx+'.description'" style="margin-bottom:0;">
                                            <a-input v-model="pst.description" placeholder="Trả góp, trả thẳng, thanh toán sản phẩm ABC, vv..vv" type="textarea" />
                                        </a-form-model-item>
                                    </template>
                                    <template slot="amount" slot-scope="text, pst, pstIdx">
                                        <a-form-model-item :rules="{required:true}" :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.transactions.'+pstIdx+'.amount'" style="margin-bottom:0;"
                                            :help="`VND: ${number_format(pst.amount || 0)}`"
                                        >
                                            <a-input-number v-model="pst.amount" style="width: 100%;" :min="-2000000000" :max="2000000000" />
                                        </a-form-model-item>
                                    </template>
                                    <template slot="paid_date" slot-scope="text, pst, pstIdx">
                                        <a-form-model-item :rules="{required:true}" :prop="'order_products.'+pIdx+'.order_product_stocks.'+psIdx+'.transactions.'+pstIdx+'.paid_date'" style="margin-bottom:0;">
                                            <a-date-picker v-model="pst.paid_date" format="YYYY-MM-DD HH:mm:ss" show-time type="date" />
                                        </a-form-model-item>
                                    </template>
                                    <template slot="action" slot-scope="text, record, pstIdx">
                                        <a-popconfirm title="Chắc chắn muốn xóa?" @confirm="() => ps.transactions.splice(pstIdx,1)">
                                            <a-button type="primary" icon="delete"></a-button>
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
                            <a-button type="primary" icon="plus"></a-button>
                        </a-tooltip>
                    </a>
                    <a-table
                        :columns="addon_transactionsTableColumns"
                        :data-source="formData.transactions"
                        :pagination="false"
                        :row-key="record => record.id"
                        bordered
                    >
                        <template slot="description" slot-scope="text, record, index">
                            <a-form-model-item :rules="{required:true}" :prop="`transactions.${index}.description`" style="margin-bottom:0;">
                                <a-input v-model="record.description" placeholder="Mã giảm giá, phí ship, v..v" type="textarea" />
                            </a-form-model-item>
                        </template>
                        <template slot="amount" slot-scope="text, record, index">
                            <a-form-model-item :rules="{required:true}" :prop="`transactions.${index}.amount`" style="margin-bottom:0;"
                                :help="`VND: ${number_format(record.amount || 0)}`"
                            >
                                <a-input-number v-model="record.amount" style="width: 100%;" :min="-2000000000" :max="2000000000" />
                            </a-form-model-item>
                        </template>
                        <template slot="paid_date" slot-scope="text, record, index">
                            <a-form-model-item :rules="{required:true}" :prop="'transactions.'+index+'.paid_date'" style="margin-bottom:0;">
                                <a-date-picker v-model="record.paid_date" format="YYYY-MM-DD HH:mm:ss" show-time type="date" />
                            </a-form-model-item>
                        </template>
                        <template slot="action" slot-scope="text, record, index">
                            <a-popconfirm title="Chắc chắn muốn xóa?" @confirm="() => formData.transactions.splice(index,1)">
                                <a-button type="primary" icon="delete" />
                            </a-popconfirm>
                        </template>
                    </a-table>
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
import OrderStatus from '../../configs/OrderStatus';
import { number_format } from '../../../helpers';

const PaidAmount = {
    props: {
        amount: Number,
        needAmount: Number,
    },

    render: function(createElement) {
        console.log(this);
        return createElement('a-tag', {
            attrs: {
                color: this.amount >= this.needAmount ? 'green' : 'red',
            },

            domProps: {
                innerHTML: number_format(this.amount),
            }
        });
    },
};

const addon_transactionsTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
        key: 'id',
    },
    {
        title: '* Nội dung',
        key: 'description',
        scopedSlots: { customRender: 'description' },
    },
    {
        title: '* Số tiền',
        key: 'amount',
        scopedSlots: { customRender: 'amount' },
    },
    {
        title: '* Ngày thanh toán',
        key: 'paid_date',
        scopedSlots: { customRender: 'paid_date' },
    },
    {
        title: 'Action',
        key: 'action',
        scopedSlots: { customRender: 'action' },
    },
];

const product_stockTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
        key: 'id',
    },
    {
        title: '* Hàng trong kho',
        key: 'stock',
        scopedSlots: { customRender: 'stock' },
    },
    {
        title: '* Tổng tiền phải thanh toán',
        key: 'amount',
        scopedSlots: { customRender: 'amount' },
    },
    {
        title: 'Action',
        key: 'action',
        scopedSlots: { customRender: 'action' },
    },
];

export default {
    data() {
        return {
            labelCol: { span: 4 },
            wrapperCol: { span: 14 },

            orderInfo: {},
            categories: [],
            productData: [],
            stockData: [],

            addon_transactionsTableColumns,
            product_stockTableColumns,

            orderInfoLoading: false,
            formData: {
                id: undefined,
                note: '',
                customer_id: 0, // TODO: config this
                order_products: [],
                status: undefined,
                transactions: [],
            },
            rules: {
                status: {required: true,},
            },
        }
    },
    components: {
        PaidAmount,
    },
    mounted() {
        this.formData.id = this.$route.params.id;

        this.loadCategoriesTree();

        if (this.formData.id) {
            this.loadOrder(this.formData.id)
        }
    },
    watch: {
        $route (to, from){
            if (!to.params.id) {
                this.formData.order_products.forEach((p) => {
                    p.id = undefined;
                    p.order_product_stocks = [];
                });

                this.formData.transactions.forEach((t) => {
                    t.id = undefined;
                });
            } else {
                this.loadOrder(this.formData.id)
            }
        },
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
                quantity: undefined,
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

        configOrderStatus() {
            return OrderStatus;
        },
    },
    methods: {
        number_format,
        // CategoriesTree
        loadCategoriesTree(){
            // this.categoriesTreeLoading = true;
            axios.get('/api/categories')
                .then(res => {
                    this.categories = res.data.data.sort((a, b) => a.parent_id - b.parent_id);

                    const len = this.categories.length;
                    for (let i = 0; i < len; i++)
                    {
                        const elm = this.categories[i];
                        if (elm.parent_id !== 0)
                        {
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
                    console.log(err);

                    if (err.response && err.response.message) {
                        this.$message.error(err.response.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    // this.categoriesTreeLoading = false;
                });
        },

        loadOrder(id){
            this.orderInfoLoading = true;
            axios.get(`/api/orders/${id}`)
                .then(res => {
                    const orderData = res.data.data;
                    if (!orderData.id) {
                        throw res;
                    }

                    this.orderInfo = orderData;

                    this.formData = {...orderData};

                    // Load tree

                    this.orderInfoLoading = false;
                })
                .catch(err => {
                    console.log(err);

                    if (err.response && err.response.message) {
                        this.$message.error(err.response.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
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
                            const orderData = res.data.data;
                            this.formData.id = res.data.data.id;

                            if (!this.formData.id)
                            {
                                throw res;
                                return;
                            }

                            this.formData = {...orderData};

                            this.$message.success('Đã sửa sản phẩm thành công');
                        })
                        .catch(err => {
                            console.log(err);

                            if (err.response && err.response.message) {
                                this.$message.error(err.response.message);
                                return;
                            }

                            this.$message.error(err.message || 'Thất bại');
                        })
                        .finally(()=>{
                            this.orderInfoLoading = false;
                        });
                }
                else
                {
                    axios.post('/api/orders', this.formData)
                        .then(res => {
                            const orderData = res.data.data;
                            this.formData.id = res.data.data.id;

                            if (!this.formData.id) {
                                throw res;
                            }

                            this.formData = {...orderData};

                            this.$message.success('Đã thêm sản phẩm thành công');
                            this.$router.push({ path: `/orders/${this.formData.id}/edit` });
                        })
                        .catch(err => {
                            console.log(err);

                            if (err.response && err.response.message) {
                                this.$message.error(err.response.message);
                                return;
                            }

                            this.$message.error(err.message || 'Thất bại');
                        })
                        .finally(()=>{
                            this.orderInfoLoading = false;
                        });
                }
            });
        },
        resetForm() {
            this.$refs.ruleForm.resetFields();
        },

        loadCategoryProducts(treeNode) {
            const targetOption = treeNode.dataRef;

            const category_id = targetOption.meta.category_id;

            return axios.get(`/api/products?category_id=${category_id}&all=true`)
                .then(res => {
                    const sData = res.data.data;

                    targetOption.children = [];

                    // Find category
                    const len = this.categories.length;
                    for (let i = 0; i < len; i++) {
                        const elm = this.categories[i];
                        if (elm.parent_id === category_id) {
                            const newOtp = {
                                isLeaf: false,
                                selectable: false,
                                value: 'cat-'+elm.id,
                                title: '[+] ' + elm.name,
                                meta: {category_id: elm.id},
                            };
                            targetOption.children.push(Object.assign({}, newOtp));
                        }
                    }

                    for (let i = 0; i < sData.length; i++) {
                        const elm = sData[i];
                        const newOtp = {
                            isLeaf: true,
                            title: elm.id +'. ' + elm.name + ' ('+number_format(elm.price)+' VND)',
                            value: elm.id,
                        };
                        targetOption.children.push(newOtp);
                    }

                    targetOption.disabled = (targetOption.children.length === 0);
                })
                .catch(err => {
                    console.log(err);

                    if (err.response && err.response.message) {
                        this.$message.error(err.response.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.productData = [...this.productData];
                });
        },

        loadCategoryStocks(treeNode) {
            const targetOption = treeNode.dataRef;

            const category_id = targetOption.meta.category_id;

            return axios.get(`/api/stocks?category_id=${category_id}&all=true`)
                .then(res => {
                    const sData = res.data.data;

                    targetOption.children = [];

                    // Find category
                    const len = this.categories.length;
                    for (let i = 0; i < len; i++) {
                        const elm = this.categories[i];
                        if (elm.parent_id === category_id)
                        {
                            const newOtp = {
                                isLeaf: false,
                                selectable: false,
                                value: 'cat-'+elm.id,
                                title: '[+] ' + elm.name,
                                meta: {category_id: elm.id},
                            };
                            targetOption.children.push(Object.assign({}, newOtp));
                        }
                    }

                    for (let i = 0; i < sData.length; i++) {
                        const elm = sData[i];
                        if (elm.quantity > 0)
                        {
                            const newOtp = {
                                isLeaf: true,
                                title: elm.id +'. ' + elm.name + ' ('+elm.idi+') ' + ' ('+number_format(elm.cost_price)+' VND)' + ' (x'+ elm.quantity +')',
                                value: elm.id,
                            };
                            targetOption.children.push(newOtp);
                        }
                    }

                    targetOption.disabled = (targetOption.children.length === 0);
                })
                .catch(err => {
                    console.log(err);

                    if (err.response && err.response.message) {
                        this.$message.error(err.response.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(() => {
                    this.stockData = [...this.stockData];
                });
        },
    },
}
</script>
