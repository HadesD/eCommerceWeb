<template>
    <div style="padding-top: 150px;">
        <!-- <vue-progress-bar /> -->
        <div style="text-align: center; margin: 15px 0;">
            <img src="/favicon.ico" /><span style="font-size:25px;font-weight:bold;vertical-align: bottom;">inPhone.vn</span>
        </div>
        <a-form
            ref="ruleForm"
            :model="formData"
            :rules="rules"
            @finish="onFinish"
            style="width: 400px;margin:0 auto;padding: 30px;"
        >
            <a-form-item name="email">
                <a-input
                    autoFocus
                    size="large"
                    placeholder="E-mail"
                    v-model:value="formData.email"
                >
                    <template #prefix>
                        <UserOutlined />
                    </template>
                </a-input>
            </a-form-item>

            <a-form-item name="password">
                <a-input-password
                    size="large"
                    placeholder="Mật khẩu"
                    v-model:value="formData.password"
                >
                    <template #prefix>
                        <LockOutlined />
                    </template>
                </a-input-password>
            </a-form-item>

            <a-form-item name="remember">
                <a-checkbox v-model:checked="formData.remember">Ghi nhớ</a-checkbox>
                <!-- <router-link
                    :to="{ name: 'recover', params: { user: 'aaa'} }"
                    class="forge-password"
                    style="float: right;"
                >Quên mật khẩu?</router-link> -->
            </a-form-item>

            <a-form-item style="margin-top:24px">
                <a-button
                    size="large" type="primary" htmlType="submit" block class="login-button"
                    :loading="loggingIn"
                >Đăng nhập</a-button>
            </a-form-item>
        </a-form>
    </div>
</template>
<script>
import { reactive, ref, } from 'vue';

import User from '../../utils/User';
import UserRole from '../../configs/UserRole';
import RequestHttp from '../../../utils/RequestHttp';
import RequestApi from '../../../utils/RequestApi';

import {
    UserOutlined, LockOutlined,
} from '@ant-design/icons-vue';

export default {
    components: {
        UserOutlined, LockOutlined,
    },
    setup(){
        const ruleForm = ref();
        const formData = reactive({
            email: undefined,
            password: undefined,
            remember: true,
        });

        return {
            ruleForm,
            formData,
        };
    },

    data() {
        return {
            loggingIn: false,
            rules: {
                email: [
                    { required: true, trigger: 'blur' },
                ],
                password: [
                    { required: true, trigger: 'blur' },
                ],
            },
        };
    },

    methods: {
        onFinish() {
            this.loggingIn = true;

            // this.$Progress.start();

            // Refresh CSRF
            RequestHttp.get('/sanctum/csrf-cookie')
                .then(async res => {
                    // Login
                    await RequestHttp.post('/login', this.formData);

                    // Check permission
                    const userApiRequest = await RequestApi.get('/user');
                    const userData = userApiRequest.data;
                    if (userData.role >= UserRole.ROLE_ADMIN_MANAGER) {
                        User.setInfo(userData);

                        // this.$Progress.finish();

                        this.$router.push({path: '/'});
                    } else {
                        this.$message.error('Bạn không có quyền hạn truy cập trang này');

                        // this.$Progress.fail();
                    }
                })
                .catch(err => {
                    // this.$Progress.fail();

                    if (err.response && err.response.data && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Thất bại');
                })
                .finally(() => {
                    this.loggingIn = false;
                });
        },
    },
}
</script>
