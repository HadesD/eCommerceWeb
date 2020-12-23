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
                    size="large"
                    type="primary"
                    htmlType="submit"
                    block
                    class="login-button"
                    :loading="loggingIn"
                    :disabled="refreshingCsrf"
                    @click="onSubmit"
                    >Đăng nhập</a-button>
            </a-form-model-item>
        </a-form-model>
    </div>
</template>
<script>
export default {
    data(){
        return {
            refreshingCsrf: false,
            loggingIn: false,

            csrf: '',
            formData: {
                email: '',
                password: '',
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
            validErrors: {},
        }
    },
    mounted(){
    },
    methods: {
        onSubmit(){
            this.$refs.ruleForm.validate(valid => {
                if (!valid)
                {
                    return false;
                }
                this.refreshingCsrf = true;
                this.$Progress.start();
                axios.get('/sanctum/csrf-cookie').then(response => {
                    this.loggingIn = true;
                    axios.post('/login', this.formData).then(res => {
                        axios.get('/api/user').then(userRes => {
                            const userData = userRes.data;
                            if (userData.role >= 100)
                            {
                                this.$user.setInfo(userData);
                                this.$Progress.finish();
                                this.$router.push({path: '/index'});
                            }
                            else
                            {
                                this.$message.error('Bạn không có quyền hạn truy cập trang này');
                                this.$Progress.fail();
                            }
                        }).catch(res => {
                            this.$message.error('Lấy thông tin người dùng bị lỗi: ' + res.message);
                            this.$Progress.fail();
                        }).then(()=> {
                            this.loggingIn = false;
                            this.$Progress.stop();
                        });
                    }).catch(error => {
                        const res = error.response;
                        const resData = res.data;
                        this.$message.error('Login thất bại: ' + resData.message);
                        this.validErrors = resData.errors;
                        this.$Progress.fail();
                    }).then(()=> {
                        this.loggingIn = false;
                        this.$Progress.stop();
                    });
                }).catch(res => {
                    this.$message.error('Get CSRF thất bại');
                    this.$Progress.fail();
                }).then(()=> {
                    this.refreshingCsrf = false;
                    this.loggingIn = false;
                    this.$Progress.stop();
                });

                return false;
            });

            return false;
        },
    }
}
</script>
