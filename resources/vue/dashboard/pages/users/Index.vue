<template>
    <div>
        <a-page-header title="Người dùng / Khách hàng">
            <template slot="tags">
                <a-tooltip title="Làm mới">
                    <a-button type="primary" icon="reload" :loading="usersTableLoading" @click="() => loadUserList({page:usersTablePagination.current})" />
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
            @change="(pagination) => loadUserList({page:pagination.current})"
        >
            <a-tag slot="role" slot-scope="value" :color="configUserRole[value].color">{{ configUserRole[value].name }}</a-tag>
            <template slot="action" slot-scope="record">
                <template v-if="!onFinishSelect">
                    <a-button type="primary" icon="edit" @click="() => {userId = record.id; editPageVisible = true;}" />
                </template>
                <template v-else>
                    <a-button type="primary" icon="user" @click="() => onFinishSelect({userId: record.id, recordData: record})">Chọn</a-button>
                </template>
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
import UserRole from '../../configs/UserRole';
import Edit from './Edit';

const usersTableColumns = [
    {
        title: '#',
        dataIndex: 'id',
    },
    {
        title: 'Tên',
        dataIndex: 'name',
    },
    {
        title: 'Chức vụ',
        dataIndex: 'role',
        scopedSlots: { customRender: 'role' },
    },
    {
        title: 'Số điện thoại',
        dataIndex: 'phone',
    },
    {
        title: 'Mạng xã hội',
        dataIndex: 'sns_info',
        scopedSlots: { customRender: 'sns_info' },
    },
    {
        title: 'Email',
        dataIndex: 'email',
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

            editPageVisible: false,
        };
    },
    mounted() {
        this.loadUserList({page: 1});
    },
    computed: {
        configUserRole() {
            return UserRole;
        },
    },
    methods: {
        loadUserList({page}) {
            this.usersTableLoading = true;

            axios.get('/api/users', {
                params: {
                    page,
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
