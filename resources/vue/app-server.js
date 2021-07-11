// import app from './app';
// import renderVueComponentToString from 'vue-server-renderer/basic';
// const renderer = require('vue-server-renderer').createRenderer();
// const { renderToString } = require('@vue/server-renderer');

// renderToString(app, () => {
//     dispatch(11);
//     // dispatch(appContent);
// });

// renderVueComponentToString(app, (err, res) => {
//     if (err) throw new Error(err);
//     dispatch(res);
// });

// new Promise((resolve, reject) => {
//     app.$router.push(context.url);
//     app.$router.onReady(() => {
//         const matchedComponents = app.$router.getMatchedComponents();
//         if (!matchedComponents.length) {
//             return reject({ code: 404 });
//         }
//         resolve(app);
//     }, reject);
// })
//     .then(app => {
//         renderVueComponentToString(app, (err, res) => {
//             if (err) throw new Error(err);
//             dispatch(res);
//         });
//     })
//     .catch((err) => {
//         dispatch(err);
//     });

//=====
import { createSSRApp } from 'vue';
import { renderToString } from '@vue/server-renderer';

// import router from './configs/router';
import MainApp from './layouts/MainApp';

const app = createSSRApp(MainApp);
// app.use(router);

(async () => {
    try {
        dispatch(await renderToString(app));
    } catch (e) {
        dispatch(e.message);
    }
})();
