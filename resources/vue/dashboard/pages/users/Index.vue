<template>
    <div>
        <a-page-header title="Người dùng / Khách hàng">
            <template slot="tags">
                <a-tooltip title="Làm mới">
                    <a-button type="primary" icon="reload" :loading="stocksTableLoading" @click="() => loadStocks(currentCategoryId, stocksTablePagination.current)" />
                </a-tooltip>
            </template>
            <template slot="extra">
                <router-link to="/stocks/new">
                    <a-tooltip title="Nhập kho">
                        <a-button type="primary" icon="plus" style="float:right;" />
                    </a-tooltip>
                </router-link>
            </template>
        </a-page-header>
        <a-table
            :columns="usersTableColumns"
            :data-source="usersTableData"
            :loading="usersTableLoading"
            :row-key="record => record.id"
            :pagination="usersTablePagination"
            @change="(pagination) => loadUserList(pagination.current)"
        >
            <a-tag slot="role" slot-scope="value" :color="configUserRole[value].color">{{ configUserRole[value].name }}</a-tag>
        </a-table>
    </div>
</template>
<script>
import UserRole from '../../configs/UserRole';

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
    data() {
        return {
            usersTableColumns,
            usersTableLoading: false,
            usersTableData: [],
            usersTablePagination: {
                position: 'both',
            },
        };
    },
    mounted() {
        this.loadUserList({page: 1})
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
