<template>
    <a-config-provider :locale="locale">
        <router-view></router-view>
    </a-config-provider>
</template>
<script>
import locale from 'ant-design-vue/lib/locale-provider/vi_VN';
import moment from 'moment';
import 'moment/locale/vi';

moment.locale('vi');

export default {
    data(){
        return {
            locale,
        };
    },

    mounted() {
        this.checkUpdate();
    },

    methods: {
        getAppVer(fromDoc) {
            return {
                css: fromDoc.getElementById('app-css').getAttribute('href').split('?id=')[1],
                script: fromDoc.getElementById('app-script').getAttribute('src').split('?id=')[1],
            };
        },
        checkUpdate() {
            let hasNewVer = false;

            axios.get('/dashboard')
                .then(res => {
                    const newAppDoc = new DOMParser().parseFromString(res.data, 'text/html');

                    const curVer = this.getAppVer(document);
                    const newVer = this.getAppVer(newAppDoc);

                    if (curVer.css !== newVer.css || curVer.script !== newVer.script) {
                        this.$notification.info({
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
                    if (err.response && err.response.data.message) {
                        this.$message.error(err.response.data.message);
                        return;
                    }

                    this.$message.error(err.message || 'Kiểm tra phiên bản mới thất bại');
                })
                .finally(() => {
                    if (!hasNewVer) {
                        setTimeout(this.checkUpdate, 60000);
                    }
                });
        },
    },
}
</script>
