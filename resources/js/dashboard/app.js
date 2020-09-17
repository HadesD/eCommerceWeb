require('../bootstrap');

import Vue from 'vue'
import Antd from 'ant-design-vue';
import VueLazyload from 'vue-lazyload'

import router from './router.js'
import MainApp from './MainApp.vue'
import VueMq from 'vue-mq'

Vue.use(Antd);
Vue.use(VueLazyload);
Vue.use(VueMq);

const app = new Vue({
    el: '#app',
    router,
    render (h) {
        return h(MainApp, {
        });
    },
});

