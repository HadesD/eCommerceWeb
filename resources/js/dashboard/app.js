require('../bootstrap');

import Vue from 'vue'
import router from './router.js'
import Error404 from './pages/Error404.vue'
import MainApp from './MainApp.vue'
import Antd from 'ant-design-vue';

Vue.use(Antd);

const app = new Vue({
    el: '#app',
    router,
    components: { MainApp },
    render (h) {
        return h(MainApp, {

        });
    },
});

