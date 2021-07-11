<template>
    <div>
        <a-page-header title="Người dùng / Khách hàng">
            <template slot="tags">
                <a-tooltip title="Làm mới">
                    <a-button type="primary" icon="reload" :loading="usersTableLoading" @click="() => loadUsers({})" />
                </a-tooltip>
            </template>
            <template slot="extra">
                <a-tooltip title="Thêm người dùng">
                    <a-button type="primary" icon="plus" @click="() => { currentUserId = null; editPageVisible = true; }" />
                </a-tooltip>
            </template>
        </a-page-header>
        <a-table
            :scroll="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300 } : {}"
            :size="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? 'small' : 'default'"
            :columns="usersTableColumns"
            :data-source="usersTableData"
            :loading="usersTableLoading"
            :row-key="record => record.id"
            :pagination="usersTablePagination"
            @change="(pagination, filters) => {
                usersTableFilters = filters;
                loadUsers({page: pagination.current});
            }"
        >
            <!-- Block Search: BEGIN -->
            <div
                slot="filterSearchBox"
                slot-scope="{ setSelectedKeys, selectedKeys, confirm, clearFilters, column }"
                style="padding: 8px"
            >
                <a-input
                    :placeholder="`Tìm ${column.title}`"
                    :value="selectedKeys[0]"
                    style="width: 188px; margin-bottom: 8px; display: block;"
                    @change="e => setSelectedKeys(e.target.value ? [e.target.value] : [])"
                    @pressEnter="() => $refs[`filterSearchBoxSubmit.${column.dataIndex}`].$el.click()"
                />
                <a-button
                    :ref="`filterSearchBoxSubmit.${column.dataIndex}`"
                    type="primary"
                    icon="search"
                    size="small"
                    style="width: 90px; margin-right: 8px"
                    @click="() => {confirm();}"
                >Tìm</a-button>
                <a-button size="small" style="width: 90px" @click="() => {setSelectedKeys([]);clearFilters();}">Reset</a-button>
            </div>
            <SearchOutlined
                slot="filterSearchBoxIcon"
                slot-scope="filtered"
                :style="{ color: filtered ? '#108ee9' : null }"
            />
            <!-- Block Search: END -->

            <template #role="{ text }">
                <a-tag v-if="configUserRole[value]" :color="configUserRole[value].color">{{ configUserRole[value].name }}</a-tag>
            </template>
            <template #action="{ record }">
                <template v-if="!onFinishSelect">
                    <a-button type="primary" icon="edit" @click="() => {currentUserId = record.id; editPageVisible = true;}" />
                </template>
                <template v-else>
                    <a-button type="primary" icon="user" @click="() => onFinishSelect(record)">Chọn</a-button>
                </template>
            </template>
            <template #time="{ record }">
                <div>Tạo: {{ date_format(record.created_at) }}</div>
                <div>Update: {{ date_format(record.updated_at) }}</div>
            </template>
            <template #sns_info="{ text }">
                <a v-if="value && value.facebook" :href="value.facebook" target="_blank">
                    <FacebookOutlined />
                </a>
            </template>
        </a-table>
        <a-modal
            :visible="editPageVisible"
            @cancel="() => editPageVisible = false"
            :footer="false"
            :width="700"
        >
            <Edit :userId="currentUserId" />
        </a-modal>
    </div>
</template>
<script>
import UserRole, { Config as configUserRole } from '../../configs/UserRole';
import { date_format } from '../../../helpers';
import RequestRepository from '../../utils/RequestRepository';
import {
    SearchOutlined, FacebookOutlined,
} from '@ant-design/icons-vue';

const usersTableColumns = [
    {
        title: '#ID',
        dataIndex: 'id',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Tên',
        dataIndex: 'name',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Chức vụ',
        dataIndex: 'role',
        slots: {
            customRender: 'role',
        },
    },
    {
        title: 'Số điện thoại',
        dataIndex: 'phone',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Mạng xã hội',
        dataIndex: 'sns_info',
        slots: {
            customRender: 'sns_info',
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Email',
        dataIndex: 'email',
        slots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Thời gian',
        key: 'time',
        slots: { customRender: 'time' },
    },
    {
        title: 'Hành động',
        key: 'action',
        slots: { customRender: 'action' },
    },
];

export default {
    props: {
        onFinishSelect: Function,
    },
    components: {
        Edit: () => import('./Edit'),
        SearchOutlined, FacebookOutlined,
    },
    data() {
        return {
            currentUserId: null,

            usersTableColumns,
            usersTableLoading: false,
            usersTableData: [],
            usersTablePagination: {
                position: 'both',
            },
            usersTableFilters: {},

            editPageVisible: false,

            UserRole,
            configUserRole,
        };
    },
    mounted() {
        this.loadUsers({page: 1});
    },
    computed: {
    },
    methods: {
        date_format,
        loadUsers({page}) {
            this.usersTableLoading = true;

            RequestRepository.get('/users', {
                params: {
                    page: page || this.usersTablePagination.current,
                    ...this.usersTableFilters,
                }
            })
                .then(res => {
                    const resData = res.data;

                    this.usersTableData = resData.data;

                    this.usersTablePagination = {
                        ...this.usersTablePagination,
                        total: resData.total,
                        current: resData.current_page,
                        pageSize: resData.per_page,
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
                    this.usersTableLoading = false;
                });
        },
    },
};
</script>
