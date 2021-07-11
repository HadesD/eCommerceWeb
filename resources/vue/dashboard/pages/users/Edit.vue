<template>
    <a-page-header
        :title="id ? 'Sửa thông tin người dùng' : 'Thêm khách hàng / người dùng mới'"
        :subTitle="id ? `#${id}` : null"
    >
        <template #tags>
            <a-tooltip title="Lấy dữ liệu mới nhất" v-if="id">
                <a-button type="primary" size="small" :loading="userInfoLoading" @click="() => loadUser(id)">
                    <template #icon><ReloadOutlined /></template>
                </a-button>
            </a-tooltip>
            <a-tooltip title="Xoá toàn bộ dữ liệu đang nhập" v-if="!userId">
                <a-popconfirm title="Xác nhận reset toàn bộ dữ liệu đang nhập?" @confirm="() => formData.id = (formData.id === undefined) ? null : undefined">
                    <a-button type="primary" danger size="small">
                        <template #icon><DeleteOutlined /></template>
                    </a-button>
                </a-popconfirm>
            </a-tooltip>
        </template>
        <a-descriptions size="small" :column="1" v-if="id">
            <a-descriptions-item label="Ngày tạo">
                <span>{{ date_format(userInfo.created_at) }}</span>
            </a-descriptions-item>
            <a-descriptions-item label="Ngày cập nhật">
                <span>{{ date_format(userInfo.updated_at) }}</span>
            </a-descriptions-item>
        </a-descriptions>
    </a-page-header>
    <a-spin :spinning="userInfoLoading">
        <a-form
            ref="ruleForm"
            :model="formData"
            :rules="rules"
            @finish="onFinish"
            layout="vertical"
        >
            <a-form-item label="Họ tên" name="name">
                <a-input v-model:value="formData.name" />
            </a-form-item>
            <a-form-item label="Số điện thoại" name="phone">
                <a-input v-model:value="formData.phone" />
            </a-form-item>
            <a-form-item label="Facebook" name="sns_info.facebook" help="https://facebook.com/zuck">
                <a-input v-model:value="formData.sns_info.facebook">
                    <template v-if="formData.sns_info.facebook" #addonAfter>
                        <a :href="formData.sns_info.facebook" target="_blank"><FacebookOutlined /></a>
                    </template>
                </a-input>
            </a-form-item>
            <a-form-item label="Email" name="email">
                <a-input v-model:value="formData.email" />
            </a-form-item>
            <a-form-item label="Mật khẩu" name="password">
                <a-input v-model:value="formData.password" />
            </a-form-item>
            <a-form-item label="Chức vụ" name="role">
                <a-select v-model:value="formData.role">
                    <a-select-option v-for="userRole in Object.keys(configUserRole)" :key="userRole" :value="parseInt(userRole)" :disabled="!authUser.hasPermission(UserRole.ROLE_ADMIN_MASTER) && (parseInt(userRole) >= UserRole.ROLE_ADMIN_MASTER)">{{ configUserRole[userRole].name }}</a-select-option>
                </a-select>
            </a-form-item>
            <a-form-item :label-col="{ span: 0 }" :wrapper-col="{ span: 24 }">
                <a-button
                    type="primary" htmlType="submit"
                    block
                    size="large"
                    :disabled="id && !authUser.hasPermission(userInfo.role)"
                >{{ id ? 'Sửa' : 'Tạo' }}</a-button>
            </a-form-item>
        </a-form>
    </a-spin>
</template>
<script>
import { reactive, ref, } from 'vue';

import {
    ReloadOutlined, DeleteOutlined,
    FacebookOutlined,
} from '@ant-design/icons-vue';

import UserRole, { Config as configUserRole } from '../../configs/UserRole';
import { date_format } from '../../../helpers';
import User from '../../utils/User';
import RequestRepository from '../../utils/RequestRepository';

export default {
    props: {
        userId: Number,
    },
    components: {
        ReloadOutlined, DeleteOutlined,
        FacebookOutlined,
    },
    setup() {
        const ruleForm = ref();
        const formData = reactive({
            id: undefined,
            name: undefined,
            email: undefined,
            phone: undefined,
            password: undefined,
            sns_info: {
                facebook: undefined,
            },
            role: UserRole.ROLE_USER_NORMAL,
        });

        return {
            ruleForm,
            formData,
        };
    },
    data() {
        return {
            userInfoLoading: false,

            userInfo: {},

            rules: {
                name: [
                    { required: true },
                ],
                phone: [
                    { required: true },
                ],
                email: [
                    { type: 'email' },
                ],
                sns_info: {
                    facebook: [
                        { type: 'url' },
                    ],
                },
            },

            UserRole,
            configUserRole,

            authUser: User.info(),
        };
    },
    mounted() {
        if (this.id) {
            this.loadUser(this.id);
        }
    },
    watch: {
        userId() {
            this.formData.id = undefined;
        },

        id(to) {
            this.formData.password = undefined;

            if (to) {
                this.loadUser(to);
            } else {
                this.userInfo = {};

                this.$refs.ruleForm.resetFields();
            }
        },
    },
    computed: {
        id() {
            return this.userId || this.formData.id;
        },
    },
    methods: {
        date_format,

        loadUser(id) {
            this.userInfoLoading = true;
            RequestRepository.get(`/users/${id}`)
                .then(res => {
                    const uData = res.data.data;

                    this.userInfo = uData;

                    _.assign(this.formData, _.pick(uData, _.keys(this.formData)));

                    this.formData.sns_info = {
                        ...this.formData.sns_info,
                    };
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(() => {
                    this.userInfoLoading = false;
                });
        },

        onFinish() {
            this.userInfoLoading = true;

            const userId = this.id;

            const request = userId ? RequestRepository.put : RequestRepository.post;
            request('/users' + (userId ? `/${userId}` : ''), {
                ...this.formData,
            })
                .then(res => {
                    const uData = res.data.data;
                    if (!uData.id) {
                        throw res;
                    }

                    this.$message.success(userId ? 'Đã sửa thành công' : 'Đã thêm thành công');

                    this.loadUser(uData.id);
                })
                .catch(err => {
                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(()=>{
                    this.userInfoLoading = false;
                });
        },
    },
};
</script>
