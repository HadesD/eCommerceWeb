import { createSSRApp } from 'vue';
import { renderToString } from '@vue/server-renderer';
import { createRouter, createMemoryHistory } from 'vue-router';

import routes from './configs/routes';
import MainApp from './layouts/MainApp';
import Antd from 'ant-design-vue';

// Fix requestAnimationFrame is not defined
global.requestAnimationFrame = cb => cb();

const app = createSSRApp(MainApp);

const router = createRouter({
    history: createMemoryHistory('/'),
    routes,
});

app.use(Antd);
app.use(router);

(async () => {
    try {
        router.push({path: context.url});
        await router.isReady();
        dispatch(await renderToString(app));
    } catch (e) {
        // throw new Error(e);
        dispatch(e.stack);
    }
})();
