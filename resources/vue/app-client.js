import app from './app';
import { createRouter, createWebHistory } from 'vue-router';

import routes from './configs/routes';
import Antd from 'ant-design-vue';

const router = createRouter({
    history: createWebHistory('/'),
    routes,
});

app.use(Antd);
app.use(router);

app.mount('#app');
