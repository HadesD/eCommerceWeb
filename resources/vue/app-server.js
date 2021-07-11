import { createSSRApp } from 'vue';
import { renderToString } from '@vue/server-renderer';
import { createRouter, createMemoryHistory } from 'vue-router';

import routes from './configs/routes';
import MainApp from './layouts/MainApp';

const app = createSSRApp(MainApp);

const router = createRouter({
    history: createMemoryHistory('/'),
    routes,
});

app.use(router);

(async () => {
    try {
        router.push({path: context.url});
        await router.isReady();
        dispatch(await renderToString(app));
    } catch (e) {
        dispatch(e.message);
    }
})();
