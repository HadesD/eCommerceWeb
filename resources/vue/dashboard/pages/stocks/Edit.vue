<template>
    <div>
        <AddCategoryModal
            :visible="addCategoryModalVisible"
            :categories="categories"
            :categoriesTreeLoading="categoriesTreeLoading"
            @handleOk="addCategoryModalHandleOk"
            @handleCancel="addCategoryModalHandleCancel"
            @updateCategories="updateCategories"
        />
        <a-spin :spinning="stockInfoLoading">
            <a-page-header
                :title="id ? `Tên hàng: ${formData.name}` : 'Nhập hàng mới vào kho'"
                :sub-title="id ? `#${id}` : false"
            >
                <template slot="tags">
                    <a-tooltip title="Lấy dữ liệu mới nhất" v-if="id">
                        <a-button type="primary" size="small" icon="reload" :loading="stockInfoLoading" @click="() => loadStock(id)" />
                    </a-tooltip>
                </template>
                <a-descriptions size="small" :column="1" v-if="id">
                    <a-descriptions-item label="Ngày tạo">
                        <span>{{ stockInfo.created_at }}</span>
                    </a-descriptions-item>
                    <a-descriptions-item label="Ngày cập nhật">
                        <span>{{ stockInfo.updated_at }}</span>
                    </a-descriptions-item>
                    <a-descriptions-item label="Người cập nhật" v-if="stockInfo.updated_user">
                        <span>{{ stockInfo.updated_user_id }}. {{ stockInfo.updated_user.name }} </span>
                        <a-button icon="search" size="small" @click="() => { currentUserId = stockInfo.updated_user_id; userEditPageVisible = true; }" />
                    </a-descriptions-item>
                    <a-descriptions-item label="Tổng chi phí (VND)">
                        <span>{{ number_format(stockInfo.transactions.reduce((accumlator, currentValue) => accumlator + currentValue.amount, 0)) }}</span>
                    </a-descriptions-item>
                </a-descriptions>
            </a-page-header>
            <a-form-model
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? {span: 4} : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? {span: 14} : {}"
            >
                <a-form-model-item label="Tên sản phẩm" prop="name">
                    <a-input v-model="formData.name" />
                </a-form-model-item>
                <a-form-model-item label="Id/Imei/Mã phân biệt" prop="idi">
                    <a-input v-model="formData.idi" />
                </a-form-model-item>
                <a-form-model-item
                    label="Số lượng"
                    prop="quantity"
                    :help="(id && (id > 0)) ?
                        ((formData.quantity !== prev_quantity) ? 'Hệ thống sẽ tự động tạo giao dịch tương ứng với hành động tăng / giảm số lượng' : false)
                        : 'Hệ thống sẽ tự động tạo giao dịch tương ứng với số lượng nhập vào kho'"
                >
                    <a-input-number v-model="formData.quantity" :min="id ? 0 : 1" :max="200" />
                </a-form-model-item>
                <a-form-model-item label="Giá nhập (Đơn giá)" prop="cost_price" :help="`VND: ${number_format(formData.cost_price || 0)}`">
                    <a-input-number
                        v-model="formData.cost_price"
                        style="width: 100%;"
                        :min="1"
                        :max="2000000000"
                        :disabled="disabledField(formData, UserRole.ROLE_ADMIN_MASTER)"
                    />
                </a-form-model-item>
                <a-form-model-item label="Giá bán dự kiến tối thiểu (Đơn giá)" prop="sell_price" :help="`VND: ${number_format(formData.sell_price || 0)}`">
                    <a-input-number
                        v-model="formData.sell_price"
                        style="width: 100%;"
                        :min="formData.cost_price"
                        :max="2000000000"
                    />
                </a-form-model-item>
                <a-form-model-item
                    label="Nhân viên chịu trách nhiệm kiểm thử" prop="tester_id"
                    :help="stockInfo.tester_id ? `Đang chọn: #${orderInfo.tester_id}. ${orderInfo.customer.name} (Phone: ${orderInfo.customer.phone || 'Chưa có'})` : false"
                >
                    <a-row :gutter="8">
                        <a-col :span="12">
                            <a-input-search v-model="formData.tester_id" readOnly @search="() => { currentUserId = formData.tester_id; userEditPageVisible = true; }">
                                <a-button icon="search" slot="enterButton" />
                            </a-input-search>
                        </a-col>
                        <a-col :span="8">
                            <a-tooltip title="Chọn từ danh sách" v-if="!id || disabledField(orderInfo, UserRole.ROLE_ADMIN_MANAGER)">
                                <a-button type="primary" icon="user" @click="() => userIndexPageVisible = true">Chọn</a-button>
                            </a-tooltip>
                        </a-col>
                    </a-row>
                </a-form-model-item>
                <a-form-model-item label="Chuyên mục cha">
                    <a-form-model-item style="display: inline-block; margin-right: 5px;">
                        <a-tooltip title="Thêm chuyên mục">
                            <a-button type="primary" icon="plus" @click="showAddCategoryModal" />
                        </a-tooltip>
                    </a-form-model-item>
                    <a-form-model-item prop="categories_id" :style="{ display: 'inline-block', width: 'calc(100% - 80px)' }">
                        <a-spin :spinning="categoriesTreeLoading">
                            <a-tree-select
                                show-search
                                allow-clear
                                multiple
                                tree-data-simple-mode
                                treeDefaultExpandAll
                                treeNodeFilterProp="title"
                                v-model="formData.categories_id"
                                style="width: 100%"
                                :dropdown-style="{ maxHeight: '400px', overflow: 'auto' }"
                                :tree-data="categoriesTreeData"
                                placeholder="Chuyên mục"
                                :replaceFields="{ pId:'parent_id',title:'name',value:'id' }"
                            />
                        </a-spin>
                    </a-form-model-item>
                    <a-form-model-item style="display: inline-block; margin-left: 5px;">
                        <a-tooltip title="Làm mới">
                            <a-button type="primary" icon="reload" @click="reloadCategoriesTree" :loading="categoriesTreeLoading" />
                        </a-tooltip>
                    </a-form-model-item>
                </a-form-model-item>
                <a-form-model-item label="Ghi chú">
                    <a-textarea v-model="formData.note" />
                </a-form-model-item>
                <a-form-model-item
                    label="Ngày nhập / trả"
                    prop="inout_date"
                    help="Ngày nhập sẽ liên kết trực tiếp tới tiền vốn của tháng đó"
                    :rules="{required:true}"
                    v-if="formData.quantity !== prev_quantity"
                >
                    <a-date-picker
                        v-model="formData.inout_date"
                        format="YYYY-MM-DD HH:mm:ss"
                        show-time
                        type="date"
                        :disabledDate="disabledLastMonthAndTomorrow"
                    />
                </a-form-model-item>
                <a-card title="Giao dịch thêm" style="margin-bottom:16px;" :headStyle="{ backgroundColor:'#9800ab',color:'#FFF' }" :bodyStyle="{padding:0}">
                    <a slot="extra" @click="() => formData.transactions.push(Object.assign({}, transaction_obj))">
                        <a-tooltip title="Thêm giao dịch">
                            <a-button type="primary" icon="plus" />
                        </a-tooltip>
                    </a>
                    <a-table
                        :scroll="(['xs','sm','md'].indexOf($mq) !== -1) ? { x: 1300, y: '85vh' } : {}"
                        size="small"
                        :columns="addon_transactionsTableColumns"
                        :data-source="formData.transactions"
                        :pagination="false"
                        :row-key="record => `addon-tnx-${record.id || Math.random()}`"
                        bordered
                    >
                        <template slot="description" slot-scope="text, record, index">
                            <a-form-model-item
                                :rules="[{required:true,message:'Không được để trống'}, {min:10,message:'Yêu cầu ghi nội dung cẩn thận (Tối thiểu 10 ký tự)'}]"
                                :prop="`transactions.${index}.description`" style="margin-bottom:0;"
                            >
                                <a-input v-model="record.description" placeholder="Mã giảm giá, phí ship, v..v" type="textarea" :disabled="disabledField(record)" />
                            </a-form-model-item>
                        </template>
                        <template slot="amount" slot-scope="text, record, index">
                            <a-form-model-item
                                :rules="{required:true,message:'Không được để trống'}"
                                :prop="`transactions.${index}.amount`"
                                style="margin-bottom:0;"
                                :help="`VND: ${number_format(record.amount || 0)}`"
                            >
                                <a-input-number v-model="record.amount" style="width: 100%;" :min="-2000000000" :max="2000000000" :disabled="disabledField(record, UserRole.ROLE_ADMIN_MASTER)" />
                            </a-form-model-item>
                        </template>
                        <template slot="paid_date" slot-scope="text, record, index">
                            <a-form-model-item
                                :rules="{required:true,message:'Không được để trống'}"
                                :prop="'transactions.'+index+'.paid_date'" style="margin-bottom:0;"
                                help="Ngày thanh toán sẽ liên kết trực tiếp tới tiền lãi/thu của tháng đó"
                            >
                                <a-date-picker
                                    v-model="record.paid_date" format="YYYY-MM-DD HH:mm:ss" show-time type="date" :disabled="disabledField(record)"
                                    :disabledDate="disabledLastMonthAndTomorrow"
                                />
                            </a-form-model-item>
                        </template>
                        <template slot="action" slot-scope="text, record, index">
                            <a-popconfirm v-if="!record.id" title="Chắc chắn muốn xóa?" @confirm="() => formData.transactions.splice(index,1)">
                                <a-button type="danger" icon="delete" />
                            </a-popconfirm>
                        </template>
                    </a-table>
                </a-card>
                <a-collapse :activeKey="['1']" style="margin-bottom: 15px;" v-if="id && (id > 0)">
                    <a-collapse-panel key="1" header="Lịch sử xuất đơn">
                        <template v-if="stockInfo.orders_history && (stockInfo.orders_history.length > 0)">
                            <div v-for="order in stockInfo.orders_history" :key="order.id">
                                <span>#{{ order.id }} ({{ configOrderStatus[order.status].name }})</span>
                                <a-button icon="search" size="small" @click="() => { currentOrderId = order.id; orderEditPageVisible = true; }" />
                            </div>
                        </template>
                        <template v-else>
                            <a-empty />
                        </template>
                    </a-collapse-panel>
                </a-collapse>
                <a-form-model-item :label-col="{ span: 0 }" :wrapper-col="{ span: 16, offset: 4 }">
                    <a-button
                        type="primary" htmlType="submit" @click="() => $refs.ruleForm.validate((valid) => { if (valid) onFinish() })"
                        block
                        size="large"
                    >{{ id ? 'Sửa' : 'Nhập kho' }}</a-button>
                </a-form-model-item>
            </a-form-model>
        </a-spin>

        <a-modal
            :visible="orderEditPageVisible"
            @cancel="() => orderEditPageVisible = false"
            :footer="false"
            width="95vw"
        >
            <OrderEdit :orderId="currentOrderId" />
        </a-modal>

        <a-modal
            :visible="userEditPageVisible"
            @cancel="() => userEditPageVisible = false"
            :footer="false"
            :width="800"
        >
            <UserEdit :userId="currentUserId" />
        </a-modal>
    </div>
</template>

<script>
import moment from 'moment';
import { number_format } from '../../../helpers';
import UserRole from '../../configs/UserRole';
import User from '../../utils/User';
import { Config as configOrderStatus } from '../../configs/OrderStatus';

const addon_transactionsTableColumns = [
    {
        title: '#ID',
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

export default {
    props: {
        stockId: Number,
    },
    components: {
        AddCategoryModal: () => import('../../components/AddCategoryModal.vue'),
        UserEdit: () => import('../users/Edit'),
        OrderEdit: () => import('../orders/Edit'),
    },
    data() {
        return {
            orderEditPageVisible: false,
            currentOrderId: undefined,

            userEditPageVisible: false,
            currentUserId: undefined,

            categoriesTreeLoading: false,
            addCategoryModalVisible: false,
            categories: [],

            addon_transactionsTableColumns,

            stockInfoLoading: false,
            stockInfo: {},
            formData: {
                id: undefined,
                name: undefined,
                idi: undefined,
                cost_price: undefined,
                sell_price: undefined,
                tester_id: undefined,
                quantity: 1,
                note: undefined,
                inout_date: undefined,
                transactions: [],
                categories_id: [],
            },
            prev_quantity: undefined,

            rules: {
                idi: [
                    { required: true },
                ],
                cost_price: [
                    { required: true },
                ],
                quantity: [
                    { required: true },
                ],
                categories_id: [
                    { required: true },
                ],
            },

            authUser: User.info(),
            UserRole,
            configOrderStatus,
        }
    },
    computed: {
        id() {
            return this.stockId || this.formData.id;
        },
        transaction_obj() {
            return {
                id: undefined,
                description: undefined,
                amount: undefined,
                paid_date: undefined,
            }
        },
        categoriesTreeData(){
            let data = this.categories;

            for (let i = 0; i < data.length; i++) {
                data[i].pId = data[i].parent_id;
            }

            return data;
        },
    },
    watch: {
        stockId() {
            this.formData.id = undefined;
        },

        id(to) {
            this.prev_quantity = undefined;

            if (to) {
                this.loadStock(to);
            } else {
                this.$refs.ruleForm.resetFields();

                this.formData.transactions = [];

                this.stockInfo = {};
            }
        },
    },
    mounted() {
        this.reloadCategoriesTree();

        if (this.id) {
            this.loadStock(this.id);
        }
    },
    methods: {
        number_format,

        disabledLastMonthAndTomorrow(current) {
            return !this.authUser.hasPermission(UserRole.ROLE_ADMIN_SUB_MASTER) &&
                current && ((current < moment().startOf('month')) || (current > moment().endOf('day')));
        },
        disabledField(record, needRole = UserRole.ROLE_ADMIN_SUB_MASTER) {
            return !this.authUser.hasPermission(needRole) && record.id && (record.id >= 0);
        },

        loadCategoriesTree(){
            this.reloadCategoriesTree();
        },
        reloadCategoriesTree(){
            this.categoriesTreeLoading = true;
            axios.get('/api/categories')
                .then(res => {
                    this.categories = res.data.data || [];
                })
                .catch(err => {
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.categoriesTreeLoading = false;
                });
        },
        updateCategories(cats) {
            this.loadCategoriesTree();
        },
        showAddCategoryModal() {
            this.addCategoryModalVisible = true;
        },
        addCategoryModalHandleOk(e) {
            // this.addCategoryModalVisible = false;
        },
        addCategoryModalHandleCancel(e) {
            this.addCategoryModalVisible = false;
        },

        loadStock(id) {
            this.stockInfoLoading = true;

            // Reset popup data
            this.currentUserId = undefined;
            this.currentOrderId = undefined,

            axios.get(`/api/stocks/${id}`)
                .then(res => {
                    const sData = res.data.data;
                    if (!sData.id) {
                        throw res;
                    }

                    _.assign(this.formData, _.pick(sData, _.keys(this.formData)));

                    this.formData.categories_id = sData.categories.map((item) => item.id);
                    this.formData.inout_date = undefined;
                    this.formData.transactions = this.formData.transactions.map(value => {
                        return {
                            ...value,
                            paid_date: moment(value.paid_date),
                        }
                    });

                    this.stockInfo = sData;
                    this.prev_quantity = sData.quantity;

                    this.stockInfoLoading = false;
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
            this.stockInfoLoading = true;

            const stockId = this.id;

            axios({
                url: '/api/stocks' + (stockId ? `/${stockId}` : ''),
                method: stockId ? 'put' : 'post',
                data: {
                    ...this.formData,
                    inout_date: moment(this.formData.inout_date).format('YYYY-MM-DD HH:mm:ss'),
                    transactions: this.formData.transactions.map(value => {
                        return {
                            ...value,
                            paid_date: moment(value.paid_date).format("YYYY-MM-DD HH:mm:ss"),
                        };
                    }),
                }
            })
                .then(res => {
                    const sData = res.data.data;

                    if (stockId) {
                        this.$message.success('Đã sửa sản phẩm thành công');

                        this.loadStock(sData.id);
                    } else {
                        this.$message.success('Đã thêm sản phẩm thành công');

                        if (this.stockId === undefined) {
                            this.$router.push({ path: `/stocks/${sData.id}/edit` });
                        }
                    }
                })
                .catch(err => {
                    this.stockInfoLoading = false;
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                });
        },
    },
}
</script>
