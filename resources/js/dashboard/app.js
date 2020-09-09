require('../bootstrap');

import Vue from 'vue'
import Antd from 'ant-design-vue';
import VueLazyload from 'vue-lazyload'

import router from './router.js'
import MainApp from './MainApp.vue'

Vue.use(Antd);
Vue.use(VueLazyload);

const app = new Vue({
    el: '#app',
    router,
    components: { MainApp },
    render (h) {
        return h(MainApp, {

        });
    },
});

