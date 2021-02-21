require('../bootstrap');

import Vue from 'vue'
import Antd from 'ant-design-vue';
import VueLazyload from 'vue-lazyload'
import VueProgressBar from 'vue-progressbar';

import router from './router.js'
import MainApp from './MainApp.vue'
import VueMq from 'vue-mq'
import UserInfo from './plugins/user-info.js'

Vue.use(Antd);
Vue.use(VueLazyload);
Vue.use(VueMq);
Vue.use(UserInfo);

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

