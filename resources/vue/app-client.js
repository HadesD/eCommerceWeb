import app from './app';
import { createRouter, createWebHistory } from 'vue-router';

import routes from './configs/routes';

const router = createRouter({
    history: createWebHistory('/'),
    routes,
});

app.use(router);

app.mount('#app');
