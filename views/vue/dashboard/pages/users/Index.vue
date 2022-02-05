<template>
    <a-page-header title="Người dùng / Khách hàng">
        <template #tags>
            <a-tooltip title="Làm mới">
                <a-button type="primary" :loading="usersTableLoading" @click="() => loadUsers()">
                    <template #icon>
                        <ReloadOutlined />
                    </template>
                </a-button>
            </a-tooltip>
        </template>
        <template #extra>
            <a-tooltip title="Thêm người dùng">
                <a-button type="primary" @click="() => { currentUserId = null; editPageVisible = true; }">
                    <template #icon>
                        <PlusOutlined />
                    </template>
                </a-button>
            </a-tooltip>
        </template>
    </a-page-header>
        <!-- :scroll="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? { x: 1300 } : {}" -->
    <a-table
        :size="(['xs', 'sm', 'md'].indexOf($grid.breakpoint) !== -1) ? 'small' : 'default'"
        :columns="usersTableColumns"
        :data-source="usersTableData"
        :loading="usersTableLoading"
        :row-key="record => record.id"
        :pagination="usersTablePagination"
        @change="(pagination, filters) => {
            usersTableFilters = filters;
            usersTablePagination = pagination;
            loadUsers();
        }"
    >
        <!-- Block Search: BEGIN -->
        <template #filterSearchBox="{ setSelectedKeys, selectedKeys, confirm, clearFilters, column }">
            <div style="padding: 8px">
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
                    size="small"
                    style="width: 90px; margin-right: 8px"
                    @click="() => {confirm();}"
                ><template #icon><SearchOutlined /></template> Tìm</a-button>
                <a-button size="small" style="width: 90px" @click="() => {setSelectedKeys([]);clearFilters();}">Reset</a-button>
            </div>
        </template>
        <template #filterSearchBoxIcon="{ filtered }">
            <SearchOutlined :style="{ color: filtered ? '#108ee9' : undefined }" />
        </template>
        <!-- Block Search: END -->

        <template #role="{ text }">
            <a-tag v-if="configUserRole[text]" :color="configUserRole[text].color">{{ configUserRole[text].name }}</a-tag>
        </template>
        <template #action="{ record }">
            <template v-if="!onFinishSelect">
                <a-button type="primary" @click="() => {currentUserId = record.id; editPageVisible = true;}">
                    <template #icon><EditOutlined /></template>
                </a-button>
            </template>
            <template v-else>
                <a-button type="primary" @click="() => onFinishSelect(record)">
                    <template #icon><UserOutlined /></template> Chọn
                </a-button>
            </template>
        </template>
        <template #time="{ record }">
            <div>Tạo: {{ date_format(record.created_at) }}</div>
            <div>Update: {{ date_format(record.updated_at) }}</div>
        </template>
        <template #sns_info="{ text }">
            <a v-if="text && text.facebook" :href="text.facebook" target="_blank">
                <FacebookOutlined style="font-size: 1.5em;" />
            </a>
        </template>
    </a-table>
    <a-modal
        :visible="editPageVisible"
        @cancel="() => editPageVisible = false"
        :footer="false"
        :width="700"
    >
        <UserEdit :userId="currentUserId" />
    </a-modal>
</template>

<script>
import { useRouter } from 'vue-router';

import {
    FacebookOutlined,
    SearchOutlined, DownloadOutlined, PlusOutlined,
    ReloadOutlined, ShoppingCartOutlined, EditOutlined,
    UserOutlined,
} from '@ant-design/icons-vue';

import UserRole, { Config as configUserRole } from '~/dashboard/configs/UserRole';
import RequestRepository from '~/dashboard/utils/RequestRepository';
import { date_format, showErrorRequestApi, defineAsyncComponent } from '~/helpers';

import UserEdit from '~/dashboard/pages/users/Edit.vue';
import { defineComponent } from '@vue/runtime-core';

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
        title: 'Hành động',
        key: 'action',
        slots: { customRender: 'action' },
    },
];

export default defineComponent({
    props: {
        onFinishSelect: Function,
    },
    components: {
        UserEdit,

        FacebookOutlined,
        SearchOutlined, DownloadOutlined, PlusOutlined,
        ReloadOutlined, ShoppingCartOutlined, EditOutlined,
        UserOutlined,
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
        if (!this.isModalMode) {
            this.usersTablePagination.current = this.$route.query.page;

            this.usersTableFilters = this.$route.query;
            delete this.usersTableFilters.sort_by;
            delete this.usersTableFilters.page;

            this.usersTableSorts = this.$route.query.sort_by;
        }

        this.loadUsers();
    },
    computed: {
        /**
         * Is on modal / import mode
         * @return bool
         */
        isModalMode() {
            console.log(this.onFinishSelect);
            return this.onFinishSelect !== undefined;
        },
    },
    methods: {
        date_format,

        loadUsers() {
            const router = useRouter();

            this.usersTableLoading = true;

            const params = {
                page: this.usersTablePagination.current,
                ...this.usersTableFilters,
            };

            if (!this.isModalMode) {
                this.$router.replace({
                    query: params,
                });
            }

            RequestRepository.get('/users', {
                params
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
                .catch(showErrorRequestApi)
                .finally(() => {
                    this.usersTableLoading = false;
                });
        },
    },
});
</script>
