require('../bootstrap');

import Vue from 'vue'
import Antd from 'ant-design-vue';
import VueProgressBar from 'vue-progressbar';
import VueMq from 'vue-mq';

import router from './configs/router.js';
import MainApp from './layouts/MainApp.vue';

Vue.use(Antd);
Vue.use(VueMq);

Vue.use(VueProgressBar, {
    color: '#9254de',
    failedColor: '#f5222d',
    height: '3px',
});

const app = new Vue({
    el: '#app',
    router,
    render: h => h(MainApp),
});

export default app;
