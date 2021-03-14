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
            locale
        }
    },
}
</script>
