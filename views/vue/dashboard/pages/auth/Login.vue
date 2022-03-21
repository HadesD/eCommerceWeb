<template>
    <div style="padding-top: 150px;">
        <div style="text-align: center; margin: 15px 0;">
            <img src="/logo-blue.png" />
        </div>
        <a-form
            :model="formData"
            :rules="rules"
            @finish="onFinish"
            style="width: 400px;margin:0 auto;padding: 30px;"
        >
            <a-form-item name="email">
                <a-input autoFocus size="large" placeholder="E-mail" v-model:value="formData.email">
                    <template #prefix>
                        <UserOutlined />
                    </template>
                </a-input>
            </a-form-item>

            <a-form-item name="password">
                <a-input-password size="large" placeholder="Mật khẩu" v-model:value="formData.password">
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
                <a-button size="large" type="primary" htmlType="submit" block class="login-button" :loading="loggingIn">Đăng nhập</a-button>
            </a-form-item>
        </a-form>
    </div>
</template>
<script>
import { defineComponent, reactive, ref, } from 'vue';
import { useRouter } from 'vue-router';

import {
    UserOutlined, LockOutlined,
} from '@ant-design/icons-vue';
import { message } from 'ant-design-vue';

import User from '~/dashboard/utils/User';
import UserRole, { hasPermission } from '~/dashboard/configs/UserRole';
import RequestHttp from '~/utils/RequestHttp';
import RequestApiRepository from '~/utils/RequestApiRepository';
import { showErrorRequestApi } from '~/helpers';

export default defineComponent({
    components: {
        UserOutlined, LockOutlined,
    },

    setup(){
        let loggingIn = ref(false);
        const router = useRouter();

        const formData = reactive({
            email: undefined,
            password: undefined,
            remember: true,
        });

        const onFinish = (values) => {
            loggingIn.value = true;

            RequestApiRepository.post('/login', values)
                .then(res => {
                    // Check permission
                    const userData = res.data;
                    if (hasPermission(userData.role, UserRole.ROLE_ADMIN_MANAGER)) {
                        User.setInfo(userData);

                        router.push({name: 'top'});

                        message.success('Đăng nhập thành công');
                    } else {
                        message.error('Bạn không có quyền hạn truy cập trang này');
                    }
                })
                .catch(err => {
                    showErrorRequestApi(err);
                })
                .finally(() => {
                    loggingIn.value = false;
                });
        };

        return {
            formData,

            onFinish,

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
});
</script>
