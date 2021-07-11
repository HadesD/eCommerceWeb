import { createApp } from 'vue';

import router from './configs/router.js';
import MainApp from './layouts/MainApp';

const app = createApp(MainApp);

app.use(router);

export default app;
