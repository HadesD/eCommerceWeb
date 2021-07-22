import { createApp } from 'vue';
import { createRouter, createWebHistory } from 'vue-router';

import routes from './configs/routes';
import MainApp from './layouts/MainApp';

import Antd from 'ant-design-vue';
import VueScreen from 'vue-screen';

const app = createApp(MainApp);

const router = createRouter({
    history: createWebHistory('/'),
    routes,
});

app.use(Antd);
app.use(VueScreen, 'bootstrap');
app.use(router);

app.mount('#app');
