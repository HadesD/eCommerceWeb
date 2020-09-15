<template>
    <a-config-provider :locale="locale">
        <router-view></router-view>
    </a-config-provider>
</template>
<script>
import locale from 'ant-design-vue/lib/locale-provider/vi_VN';

export default {
    data(){
        return {
            locale
        }
    },
}
</script>
