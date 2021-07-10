import Vue from 'vue';

import router from './configs/router.js';
import MainApp from './layouts/MainApp';

const app = new Vue({
    router,
    render: h => h(MainApp),
});

export default app;
