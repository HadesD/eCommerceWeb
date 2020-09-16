<template>
  <div style="padding-top: 150px;">
    <div style="text-align: center; margin: 15px 0;">
      <img src="/favicon.ico" /><span style="font-size:25px;font-weight:bold;vertical-align: bottom;">inPhone.vn</span>
    </div>
    <a-form-model
      ref="ruleForm"
      :model="formData"
      :rules="rules"
      style="width: 400px;margin:0 auto;"
      >
      <a-form-model-item ref="email" prop="email">
        <a-input
          size="large"
          type="text"
          placeholder="E-mail"
          v-model="formData.email"
          @blur="() => $refs.email.onFieldBlur()"
          >
          <a-icon slot="prefix" type="user" :style="{ color: 'rgba(0,0,0,.25)' }"/>
        </a-input>
      </a-form-model-item>

      <a-form-model-item ref="password" prop="password">
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
        axios.get('/sanctum/csrf-cookie').then(response => {
          this.loggingIn = true;
          axios.post('/login', this.formData).then(res => {
            console.log(res)

            this.$router.push({path: '/index'});
          }).catch(res => {
            this.$message.error('Login thất bại:' + res.message);
          }).then(()=> {
            this.loggingIn = false;
          });
        }).catch(res => {
          this.$message.error('Get CSRF thất bại');
        }).then(()=> {
          this.refreshingCsrf = false;
        });

        return false;
      });

      return false;
    },
  }
}
</script>
