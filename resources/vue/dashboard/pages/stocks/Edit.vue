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
                <template #tags>
                    <a-tooltip title="Lấy dữ liệu mới nhất" v-if="id">
                        <a-button type="primary" size="small" :loading="stockInfoLoading" @click="() => loadStock(id)">
                            <template #icon><ReloadOutlined /></template>
                        </a-button>
                    </a-tooltip>
                    <a-tooltip title="Xoá toàn bộ dữ liệu đang nhập" v-if="!stockId">
                        <a-popconfirm title="Xác nhận reset toàn bộ dữ liệu đang nhập?" @confirm="() => formData.id = (formData.id === undefined) ? null : undefined">
                            <a-button type="primary" danger size="small">
                                <template #icon><DeleteOutlined /></template>
                            </a-button>
                        </a-popconfirm>
                    </a-tooltip>
                </template>
                <a-descriptions size="small" :column="1" v-if="stockInfo.id">
                    <a-descriptions-item label="Ngày tạo">
                        <span>{{ date_format(stockInfo.created_at) }}</span>
                    </a-descriptions-item>
                    <a-descriptions-item label="Ngày cập nhật">
                        <span>{{ date_format(stockInfo.updated_at) }}</span>
                    </a-descriptions-item>
                    <a-descriptions-item label="Người cập nhật" v-if="stockInfo.updated_user">
                        <span>{{ stockInfo.updated_user_id }}. {{ stockInfo.updated_user.name }} </span>
                        <a-button size="small" @click="() => { currentUserId = stockInfo.updated_user_id; userEditPageVisible = true; }">
                            <template #icon><SearchOutlined /></template>
                        </a-button>
                    </a-descriptions-item>
                    <a-descriptions-item label="Tổng chi phí (VND)">
                        <span>{{ number_format(stockInfo.transactions.reduce((accumlator, currentValue) => accumlator + currentValue.amount, 0)) }}</span>
                    </a-descriptions-item>
                </a-descriptions>
            </a-page-header>
            <a-form
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                @finish="onFinish"
                :label-col="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) === -1) ? {span: 4} : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) === -1) ? {span: 14} : {}"
            >
                <a-form-item label="Tên sản phẩm" name="name">
                    <a-input v-model:value="formData.name" />
                </a-form-item>
                <a-form-item label="Id/Imei/Mã phân biệt" name="idi">
                    <a-input v-model:value="formData.idi" />
                </a-form-item>
                <a-form-item
                    label="Số lượng"
                    name="quantity"
                    :help="(id && (id > 0)) ?
                        ((formData.quantity !== prev_quantity) ? 'Hệ thống sẽ tự động tạo giao dịch tương ứng với hành động tăng / giảm số lượng' : false)
                        : 'Hệ thống sẽ tự động tạo giao dịch tương ứng với số lượng nhập vào kho'"
                >
                    <a-input-number v-model:value="formData.quantity" :min="id ? 0 : 1" :max="200" />
                </a-form-item>
                <a-form-item
                    label="Giá nhập (Đơn giá)" name="cost_price"
                    :help="`Xem trước: ${number_format(formData.cost_price || 0)} ₫`"
                >
                    <a-input-number
                        v-model:value="formData.cost_price"
                        style="width: 100%;"
                        :min="1"
                        :max="2000000000"
                        :disabled="disabledField(formData, UserRole.ROLE_ADMIN_MASTER)"
                    />
                </a-form-item>
                <a-form-item label="Giá bán dự kiến tối thiểu (Đơn giá)" name="sell_price"
                    :help="`Xem trước: ${number_format(formData.sell_price || 0)} ₫`"
                >
                    <a-input-number
                        v-model:value="formData.sell_price"
                        style="width: 100%;"
                        :min="formData.cost_price"
                        :max="2000000000"
                    />
                </a-form-item>
                <a-form-item
                    label="Người kiểm thử" name="tester_id"
                    :help="stockInfo.tester_id ? `Đang chọn: #${stockInfo.tester_id}. ${stockInfo.tester.name} (Phone: ${stockInfo.tester.phone || 'Chưa có'})` : false"
                >
                    <a-row :gutter="8">
                        <a-col :span="12">
                            <a-input-search v-model:value="formData.tester_id" readOnly @search="() => { currentUserId = formData.tester_id; userEditPageVisible = true; }">
                                <template #enterButton>
                                    <a-button>
                                        <template #icon><SearchOutlined /></template>
                                    </a-button>
                                </template>
                            </a-input-search>
                        </a-col>
                        <a-col :span="8">
                            <a-tooltip title="Chọn từ danh sách" v-if="!id || !disabledField(stockInfo, UserRole.ROLE_ADMIN_MANAGER)">
                                <a-button type="primary" @click="() => userIndexPageVisible = true"><template #icon><UserOutlined /></template> Chọn</a-button>
                            </a-tooltip>
                        </a-col>
                    </a-row>
                </a-form-item>
                <a-form-item label="Chuyên mục cha">
                    <a-form-item style="display: inline-block; margin-right: 5px;">
                        <a-tooltip title="Thêm chuyên mục">
                            <a-button type="primary" @click="showAddCategoryModal">
                                <template #icon><PlusOutlined /></template>
                            </a-button>
                        </a-tooltip>
                    </a-form-item>
                    <a-form-item name="categories_id" :style="{ display: 'inline-block', width: 'calc(100% - 80px)' }">
                        <a-spin :spinning="categoriesTreeLoading">
                            <a-tree-select
                                show-search
                                allow-clear
                                multiple
                                tree-data-simple-mode
                                treeDefaultExpandAll
                                treeNodeFilterProp="title"
                                v-model:value="formData.categories_id"
                                style="width: 100%"
                                :dropdown-style="{ maxHeight: '400px', overflow: 'auto' }"
                                :tree-data="categoriesTreeData"
                                placeholder="Chuyên mục"
                                :replaceFields="{ pId:'parent_id',title:'name',value:'id' }"
                            />
                        </a-spin>
                    </a-form-item>
                    <a-form-item style="display: inline-block; margin-left: 5px;">
                        <a-tooltip title="Làm mới">
                            <a-button type="primary" @click="reloadCategoriesTree" :loading="categoriesTreeLoading">
                                <template #icon><ReloadOutlined /></template>
                            </a-button>
                        </a-tooltip>
                    </a-form-item>
                </a-form-item>
                <a-form-item label="Ghi chú">
                    <a-textarea v-model:value="formData.note" />
                </a-form-item>
                <a-form-item
                    label="Ngày nhập / trả"
                    name="inout_date"
                    help="Ngày nhập sẽ liên kết trực tiếp tới tiền vốn của tháng đó"
                    :rules="{required:true}"
                    v-if="formData.quantity !== prev_quantity"
                >
                    <a-date-picker
                        v-model:value="formData.inout_date"
                        format="YYYY-MM-DD HH:mm:ss"
                        show-time
                        type="date"
                        :disabledDate="disabledLastMonthAndTomorrow"
                    />
                </a-form-item>
                <a-card title="Giao dịch thêm" style="margin-bottom:16px;" :headStyle="{ backgroundColor:'#9800ab',color:'#FFF' }" :bodyStyle="{padding:0}">
                    <template #extra>
                        <a @click="() => formData.transactions.push(Object.assign({}, transaction_obj))">
                            <a-tooltip title="Thêm giao dịch">
                                <a-button type="primary">
                                    <template #icon><PlusOutlined /></template>
                                </a-button>
                            </a-tooltip>
                        </a>
                    </template>
                    <a-table
                        :scroll="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300, y: '85vh' } : {}"
                        size="small"
                        :columns="addon_transactionsTableColumns"
                        :data-source="formData.transactions"
                        :pagination="false"
                        :row-key="record => `addon-tnx-${record.id || Math.random()}`"
                        bordered
                    >
                        <template #description="{ text, record, index }">
                            <a-form-item
                                :rules="[
                                    {required:true,message:'Không được để trống'},
                                    {min:(record.id && (stockInfo.transactions[index].description.indexOf(`#${record.id}`) === -1) ? 0 : 10),message:'Yêu cầu ghi nội dung cẩn thận (Tối thiểu 10 ký tự)'}
                                ]"
                                :name="['transactions', index, 'description']" style="margin-bottom:0;"
                            >
                                <a-input v-model:value="record.description" placeholder="Mã giảm giá, phí ship, v..v" type="textarea" :disabled="disabledField(record)" />
                            </a-form-item>
                        </template>
                        <template #amount="{ text, record, index }">
                            <a-form-item
                                :rules="[{required:true,message:'Không được để trống'},{type:'integer'}]"
                                :name="['transactions', index,'amount']"
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
                                    v-model:value="record.paid_date" format="YYYY-MM-DD HH:mm:ss" show-time type="date" :disabled="disabledField(record)"
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
                <a-collapse :activeKey="['1']" style="margin-bottom: 15px;" v-if="id && (id > 0)">
                    <a-collapse-panel key="1" header="Lịch sử xuất đơn">
                        <template v-if="stockInfo.orders_history && (stockInfo.orders_history.length > 0)">
                            <div v-for="order in stockInfo.orders_history" :key="order.id">
                                <span>#{{ order.id }} ({{ configOrderStatus[order.status].name }})</span>
                                <a-button size="small" @click="() => { currentOrderId = order.id; orderEditPageVisible = true; }">
                                    <template #icon><SearchOutlined /></template>
                                </a-button>
                            </div>
                        </template>
                        <template v-else>
                            <a-empty />
                        </template>
                    </a-collapse-panel>
                </a-collapse>
                <a-form-item :label-col="{ span: 0 }" :wrapper-col="{ span: 16, offset: (['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? 0 : 4 }">
                    <a-button type="primary" htmlType="submit" block size="large">{{ id ? 'Sửa' : 'Nhập kho' }}</a-button>
                </a-form-item>
            </a-form>
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
            :visible="userIndexPageVisible"
            @cancel="() => userIndexPageVisible = false"
            :footer="false"
            width="98vw"
        >
            <UserIndex :onFinishSelect="onFinishSelectUser" />
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
import { defineAsyncComponent, reactive, ref } from 'vue';
import moment from 'moment';

import {
    UserOutlined, SearchOutlined,
    PlusOutlined, ReloadOutlined, DeleteOutlined,
} from '@ant-design/icons-vue';

import { number_format, date_format } from '../../../helpers';
import UserRole from '../../configs/UserRole';
import User from '../../utils/User';
import { Config as configOrderStatus } from '../../configs/OrderStatus';
import RequestRepository from '../../utils/RequestRepository';

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

export default {
    props: {
        stockId: Number,
    },
    components: {
        AddCategoryModal: defineAsyncComponent(() => import('../../components/AddCategoryModal.vue')),
        UserIndex: defineAsyncComponent(() => import('../users/Index')),
        UserEdit: defineAsyncComponent(() => import('../users/Edit')),
        OrderEdit: defineAsyncComponent(() => import('../orders/Edit')),

        UserOutlined, SearchOutlined,
        PlusOutlined, ReloadOutlined, DeleteOutlined,
    },

    setup() {
        const ruleForm = ref();
        const formData = reactive({
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
        });

        return {
            ruleForm,
            formData,
        };
    },

    data() {
        return {
            orderEditPageVisible: false,
            currentOrderId: undefined,

            userIndexPageVisible: false,
            userEditPageVisible: false,
            currentUserId: undefined,

            categoriesTreeLoading: false,
            addCategoryModalVisible: false,
            categories: [],

            addon_transactionsTableColumns,

            stockInfoLoading: false,
            stockInfo: {},
            prev_quantity: undefined,

            rules: {
                idi: [
                    { required: true },
                ],
                name: [
                    { required: true },
                ],
                cost_price: [
                    { required: true },
                    { type: 'integer' },
                ],
                sell_price: [
                    { required: true },
                    { type: 'integer' },
                ],
                tester_id: [
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
        date_format,

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
            RequestRepository.get('/categories')
                .then(res => {
                    this.categories = res.data.data || [];
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
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

            RequestRepository.get(`/stocks/${id}`)
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
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                });
        },

        onFinish() {
            this.stockInfoLoading = true;

            const stockId = this.id;

            const request = stockId ? RequestRepository.put : RequestRepository.post;
            request('/stocks' + (stockId ? `/${stockId}` : ''), {
                ...this.formData,
                inout_date: moment(this.formData.inout_date).format('YYYY-MM-DD HH:mm:ss'),
                transactions: this.formData.transactions.map(value => {
                    return {
                        ...value,
                        paid_date: moment(value.paid_date).format("YYYY-MM-DD HH:mm:ss"),
                    };
                }),
            })
                .then(res => {
                    const sData = res.data.data;

                    this.$message.success(stockId ? 'Đã sửa sản phẩm thành công' : 'Đã thêm sản phẩm thành công');

                    this.loadStock(sData.id);
                })
                .catch(err => {
                    this.stockInfoLoading = false;

                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                });
        },

        onFinishSelectUser(recordData) {
            this.formData.tester_id = recordData.id;

            this.userIndexPageVisible = false;
        },
    },
}
</script>
