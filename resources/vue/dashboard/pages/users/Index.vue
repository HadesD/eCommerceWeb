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
                    <a-button type="primary" icon="plus" @click="() => {userId = undefined; editPageVisible = true;}" />
                </a-tooltip>
            </template>
        </a-page-header>
        <a-table
            :columns="usersTableColumns"
            :data-source="usersTableData"
            :loading="usersTableLoading"
            :row-key="record => record.id"
            :pagination="usersTablePagination"
            @change="(pagination, filters) => {usersTableFilters = filters;loadUsers({page: pagination.current});}"
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
            <a-icon
                slot="filterSearchBoxIcon"
                slot-scope="filtered"
                type="search"
                :style="{ color: filtered ? '#108ee9' : undefined }"
            />
            <!-- Block Search: END -->

            <a-tag slot="role" slot-scope="value" :color="configUserRole[value].color">{{ configUserRole[value].name }}</a-tag>
            <template slot="action" slot-scope="record">
                <template v-if="!onFinishSelect">
                    <a-button type="primary" icon="edit" @click="() => {userId = record.id; editPageVisible = true;}" />
                </template>
                <template v-else>
                    <a-button type="primary" icon="user" @click="() => onFinishSelect(record)">Chọn</a-button>
                </template>
            </template>
            <template slot="time" slot-scope="record">
                <div>Tạo: {{ date_format(record.created_at) }}</div>
                <div>Update: {{ date_format(record.updated_at) }}</div>
            </template>
            <template slot="sns_info" slot-scope="value">
                <a v-if="value && value.facebook" :href="value.facebook" target="_blank">
                    <a-icon type="facebook" style="font-size: 20px;"></a-icon>
                </a>
            </template>
        </a-table>
        <a-modal
            :visible="editPageVisible"
            @cancel="() => editPageVisible = false"
            :footer="false"
            :width="700"
        >
            <Edit :userId="userId" />
        </a-modal>
    </div>
</template>
<script>
import UserRole, { Config as configUserRole } from '../../configs/UserRole';
import Edit from './Edit';

import { date_format } from '../../../helpers';

const usersTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
    },
    {
        title: 'Tên',
        dataIndex: 'name',
        scopedSlots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Chức vụ',
        dataIndex: 'role',
        scopedSlots: {
            customRender: 'role',
        },
    },
    {
        title: 'Số điện thoại',
        dataIndex: 'phone',
        scopedSlots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Mạng xã hội',
        dataIndex: 'sns_info',
        scopedSlots: {
            customRender: 'sns_info',
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Email',
        dataIndex: 'email',
        scopedSlots: {
            filterDropdown: 'filterSearchBox',
            filterIcon: 'filterSearchBoxIcon',
        },
    },
    {
        title: 'Thời gian',
        key: 'time',
        scopedSlots: { customRender: 'time' },
    },
    {
        title: 'Hành động',
        key: 'action',
        scopedSlots: { customRender: 'action' },
    },
];

export default {
    props: {
        onFinishSelect: Function,
    },
    components: {
        Edit,
    },
    data() {
        return {
            userId: undefined,

            usersTableColumns,
            usersTableLoading: false,
            usersTableData: [],
            usersTablePagination: {
                position: 'both',
            },
            usersTableFilters: {},

            editPageVisible: false,
        };
    },
    mounted() {
        this.loadUsers({page: 1});
    },
    computed: {
        UserRole() {
            return UserRole;
        },
        configUserRole() {
            return configUserRole;
        },
    },
    methods: {
        date_format,
        loadUsers({page}) {
            this.usersTableLoading = true;

            axios.get('/api/users', {
                params: {
                    page: page || this.usersTablePagination.current,
                    ...this.usersTableFilters,
                }
            })
                .then(res => {
                    this.usersTableData = res.data.data;
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
