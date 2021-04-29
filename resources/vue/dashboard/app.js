require('../bootstrap');

import { createApp } from 'vue';
import Antd from 'ant-design-vue';
import VueProgressBar from 'vue-progressbar';
import VueMq from 'vue-mq';

import router from './configs/router.js';
import MainApp from './layouts/MainApp.vue';

const app = createApp(MainApp);

app.use(Antd);

// app.use(VueMq);

// app.use(VueProgressBar, {
//     color: '#9254de',
//     failedColor: '#f5222d',
//     height: '3px',
// });

app.use(router);

app.mount('#app');

export default app;
