<template>
    <a-config-provider :locale="locale">
        <Main>
            <router-view></router-view>
        </Main>
    </a-config-provider>
</template>
<script>
import locale from 'ant-design-vue/lib/locale-provider/vi_VN';

import Main from './layouts/Main.vue'
export default {
    components: {
        Main
    },
    data(){
        return {
            locale
        }
    },
}
</script>
