import { createSSRApp } from 'vue';
import { renderToString } from '@vue/server-renderer';
import { createRouter, createMemoryHistory } from 'vue-router';

import routes from './configs/routes';
import MainApp from './layouts/MainApp';

import initApp from './initApp';

// Fix requestAnimationFrame is not defined
global.requestAnimationFrame = cb => cb();

const app = createSSRApp(MainApp);

const router = createRouter({
    history: createMemoryHistory('/'),
    routes,
});

app.use(router);
initApp(app);

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
