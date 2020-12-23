require('../bootstrap');

import Vue from 'vue'
import Antd from 'ant-design-vue';
import VueLazyload from 'vue-lazyload'

import router from './router.js'
import MainApp from './MainApp.vue'
import VueMq from 'vue-mq'
import UserInfo from './plugins/user-info.js'
import VueProgressBar from 'vue-progressbar';

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
    render (h) {
        return h(MainApp, {
        });
    },
});

