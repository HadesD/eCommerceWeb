<template>
    <div style="padding-top: 150px;">
        <vue-progress-bar />
        <div style="text-align: center; margin: 15px 0;">
            <img src="/favicon.ico" /><span style="font-size:25px;font-weight:bold;vertical-align: bottom;">inPhone.vn</span>
        </div>
        <a-form-model
            ref="ruleForm"
            :model="formData"
            :rules="rules"
            style="width: 400px;margin:0 auto;"
        >
            <a-form-model-item ref="email" prop="email" :validateStatus="validErrors.email ? 'error' : 'validating'">
                <a-input
                    autoFocus
                    size="large"
                    type="text"
                    placeholder="E-mail"
                    v-model="formData.email"
                    @blur="() => $refs.email.onFieldBlur()"
                >
                    <a-icon slot="prefix" type="user" :style="{ color: 'rgba(0,0,0,.25)' }"/>
                </a-input>
            </a-form-model-item>

            <a-form-model-item ref="password" prop="password" :validateStatus="validErrors.password ? 'error' : 'validating'">
                <a-input-password
                    size="large"
                    placeholder="Mật khẩu"
                    v-model="formData.password"
                    @blur="() => $refs.password.onFieldBlur()"
                >
                    <a-icon slot="prefix" type="lock" :style="{ color: 'rgba(0,0,0,.25)' }"/>
                </a-input-password>
            </a-form-model-item>

            <a-form-model-item>
                <a-checkbox v-model="formData.remember">Ghi nhớ</a-checkbox>
                <router-link
                    :to="{ name: 'recover', params: { user: 'aaa'} }"
                    class="forge-password"
                    style="float: right;"
                >Quên mật khẩu?</router-link>
            </a-form-model-item>

            <a-form-model-item style="margin-top:24px">
                <a-button
                    size="large" type="primary" htmlType="submit" block class="login-button"
                    :loading="loggingIn"
                    @click="() => $refs.ruleForm.validate(valid => {if (valid) onFinish()})"
                >Đăng nhập</a-button>
            </a-form-model-item>
        </a-form-model>
    </div>
</template>
<script>
import User from '../../utils/User';

export default {
    data(){
        return {
            loggingIn: false,

            formData: {
                email: undefined,
                password: undefined,
                remember: true,
            },
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
    mounted(){
    },
    methods: {
        async onFinish(){
            try {
                this.$Progress.start();

                // Refresh CSRF
                await axios.get('/sanctum/csrf-cookie');

                // Login
                await axios.post('/login', this.formData);

                // Check permission
                const userData = await axios.get('/api/user');
                if (userData.role >= 100) {
                    User.setInfo(userData);

                    this.$Progress.finish();
                    this.$router.push({path: '/'});
                } else {
                    this.$message.error('Bạn không có quyền hạn truy cập trang này');
                    this.$Progress.fail();
                }
            } catch (err) {
                this.$Progress.fail();

                if (err.response && err.response.message) {
                    this.$message.error(err.response.message);
                    return;
                }

                this.$message.error(err.message || 'Thất bại');
            } finally {
                this.$Progress.stop();

                this.loggingIn = false;
            }

            return false;
        }
    },
}
</script>
