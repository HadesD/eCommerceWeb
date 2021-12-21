<template>
    <a-config-provider :locale="locale">
        <a-layout>
            <Header />
            <a-layout-content style="margin: 25px 0;">
                <div :style="{ background: '#fff', padding: '24px', }">
                    <router-view></router-view>
                </div>
            </a-layout-content>
            <Footer />
        </a-layout>
    </a-config-provider>
</template>

<script>
import Header from './Header';
import Footer from './Footer';

import locale from 'ant-design-vue/lib/locale-provider/vi_VN';

import RequestRepository from '../utils/RequestRepository';

export default {
    components: {
        Header, Footer,
    },

    setup() {

        return {
            locale,
        };
    },

}
</script>
