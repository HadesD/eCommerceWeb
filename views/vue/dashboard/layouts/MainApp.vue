<template>
    <a-config-provider :locale="locale">
        <router-view></router-view>
    </a-config-provider>
</template>
<script>
import locale from 'ant-design-vue/lib/locale-provider/vi_VN';
import dayjs from 'dayjs';
import 'dayjs/locale/vi';
dayjs.locale('vi');

import User from '~/dashboard/utils/User';
import RequestHttp from '~/utils/RequestHttp';
import RequestApi from '~/utils/RequestApi';

import { showErrorRequestApi } from '~/helpers';
import { message, notification } from 'ant-design-vue';
import { useRoute, useRouter } from 'vue-router';
import { onMounted } from '@vue/runtime-core';

export default {
    setup() {
        const route = useRoute();
        const router = useRouter();

        router.isReady().then(v => {
            if (route.name !== 'login') {

                const authUser = User.info();

                const msgKey = 'dashboard.user.info.reload';
                const hide = message.loading({ content: 'Đang đồng bộ thông tin đăng nhập...', key: msgKey, duration: 0 });

                RequestApi.get('/user')
                    .then(res => {
                        User.setInfo(res.data);

                        message.success({ content: 'Đồng bộ thành công', key: msgKey });
                    })
                    .catch(err => {
                        if (err.response) {
                            if (err.response.status === 401) {
                                if (authUser.id) {
                                    User.clear();
                                }

                                router.push({name: 'login'});

                                message.error({ content: 'Chưa đăng nhập', key: msgKey });

                                return;
                            }
                        }

                        message.error({ content: 'Có lỗi xảy ra', key: msgKey });

                        showErrorRequestApi(err);
                    })
                    .finally(() => setTimeout(hide, 1500));
            }
        });

        const getAppVer = (fromDoc) => {
            return {
                css: fromDoc.getElementById('app-css')?.getAttribute('href').split('?id=')[1],
                script: fromDoc.getElementById('app-js')?.getAttribute('src').split('?id=')[1],
            };
        };

        const checkUpdate = () => {
            let hasNewVer = false;

            RequestHttp.get('/dashboard')
                .then(res => {
                    const newAppDoc = new DOMParser().parseFromString(res.data, 'text/html');

                    const curVer = getAppVer(document);
                    const newVer = getAppVer(newAppDoc);

                    if ((curVer.css !== newVer.css) || (curVer.script !== newVer.script)) {
                        notification.info({
                            message: 'Có phiên bản mới của website',
                            description: 'Cần tải lại trang để cập nhật',
                            duration: 0,
                            placement: 'bottomLeft',
                            closeIcon: 'Tải lại',
                            onClose: function (){
                                location.reload();
                            },
                        });

                        hasNewVer = true;
                    }
                })
                .catch(err => {
                    if (User.info().id) {
                        showErrorRequestApi(err);
                    }
                })
                .finally(() => {
                    if (!hasNewVer) {
                        setTimeout(checkUpdate, 60000);
                    }
                });
        };

        onMounted(() => checkUpdate);

        return {
            locale,
        };
    },
}
</script>
