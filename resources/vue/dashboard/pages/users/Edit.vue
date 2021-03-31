<template>
    <div>
        <a-page-header
            :title="id ? 'Sửa thông tin người dùng' : 'Thêm khách hàng / người dùng mới'"
            :subTitle="id ? `#${id}` : null"
        />
        <a-spin :spinning="isLoadingUserInfo">
            <a-form-model
                ref="ruleForm"
                :model="formData"
                :rules="rules"
                :label-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? { span: 4 } : {}"
                :wrapper-col="(['xs', 'sm', 'md'].indexOf($mq) === -1) ? { span: 20 } : {}"
            >
                <a-form-model-item label="Họ tên" prop="name">
                    <a-input v-model="formData.name" />
                </a-form-model-item>
                <a-form-model-item label="Số điện thoại" prop="phone">
                    <a-input v-model="formData.phone" />
                </a-form-model-item>
                <a-form-model-item label="Facebook" prop="sns_info.facebook" help="https://facebook.com/zuck">
                    <a-input v-model="formData.sns_info.facebook" />
                </a-form-model-item>
                <a-form-model-item label="Email" prop="email">
                    <a-input v-model="formData.email" />
                </a-form-model-item>
                <a-form-model-item label="Mật khẩu" prop="password">
                    <a-input v-model="formData.password" />
                </a-form-model-item>
                <a-form-model-item label="Chức vụ" prop="role">
                    <a-select v-model="formData.role">
                        <a-select-option v-for="userRole in Object.keys(configUserRole)" :key="userRole" :value="parseInt(userRole)" :disabled="((authUser.role < 200) && (parseInt(userRole) >= 200))">{{ configUserRole[userRole].name }}</a-select-option>
                    </a-select>
                </a-form-model-item>
                <a-form-model-item :wrapper-col="{ span: 14, offset: 4 }">
                    <a-button type="primary" htmlType="submit" @click="() => $refs.ruleForm.validate((valid) => { if (valid) onFinish() })">
                        {{ id ? 'Sửa' : 'Tạo' }}
                    </a-button>
                </a-form-model-item>
            </a-form-model>
        </a-spin>
    </div>
</template>
<script>
import UserRole, { Config as configUserRole } from '../../configs/UserRole';
import User from '../../utils/User';

export default {
    props: {
        userId: Number,
    },
    data() {
        return {
            isLoadingUserInfo: false,
            formData: {
                name: undefined,
                email: undefined,
                phone: undefined,
                password: undefined,
                sns_info: {
                    facebook: undefined,
                },
                role: 50,
            },
            rules: {
                name: [
                    { required: true },
                ],
                email: [
                    { type: 'email' },
                ],
                phone: [
                    { required: true },
                ],
                sns_info: {
                    facebook: [
                        { type: 'url' },
                    ],
                },
            },
        };
    },
    mounted: function() {
        this.loadUserInfo();
    },
    watch: {
        id: function () {
            this.loadUserInfo();
        },
    },
    computed: {
        id() {
            return this.userId;
        },
        UserRole() {
            return UserRole;
        },
        configUserRole() {
            return configUserRole;
        },

        authUser() {
            return User.info();
        },
    },
    methods: {
        loadUserInfo() {
            this.$refs.ruleForm.resetFields();
            if (!this.id) {
                return;
            }
            this.isLoadingUserInfo = true;
            axios.get(`/api/users/${this.id}`)
                .then(res => {
                    const uData = res.data.data;

                    _.assign(this.formData, _.pick(uData, _.keys(this.formData)));

                    this.formData.sns_info = {
                        ...this.formData.sns_info,
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
                    this.isLoadingUserInfo = false;
                });
        },

        onFinish() {
            this.isLoadingUserInfo = true;

            const userId = this.id;

            axios({
                url: userId ? `/api/users/${userId}` : '/api/users',
                method: userId ? 'put' : 'post',
                data: {
                    ...this.formData,
                }
            })
                .then(res => {
                    const uData = res.data.data;
                    if (!uData.id) {
                        throw res;
                    }

                    if (userId) {
                        this.$message.success('Đã sửa thành công');
                    } else {
                        this.$message.success('Đã thêm thành công');

                        // this.$router.push({ path: `/stocks/${sData.id}/edit` });
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
                    this.isLoadingUserInfo = false;
                });
        },
    },
};
</script>
