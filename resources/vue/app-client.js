import { createApp } from 'vue';
import { createRouter, createWebHistory } from 'vue-router';

import routes from './configs/routes';
import MainApp from './layouts/MainApp';

import initApp from './initApp';

const app = createApp(MainApp);

const router = createRouter({
    history: createWebHistory('/'),
    routes,
});

app.use(router);
initApp(app);

app.mount('#app');
