import app from './app';
import renderVueComponentToString from 'vue-server-renderer/basic';

app.$router.push(context.url);

renderVueComponentToString(app, (err, res) => {
    if (err) throw new Error(err);
    dispatch(res);
});

// new Promise((resolve, reject) => {
//     router.push(context.url);
//     router.onReady(() => {
//         resolve(app);
//     }, reject);
// })
//     .then(app => {
//         renderVueComponentToString(app, (err, res) => {
//             if (err) throw new Error(err);
//             dispatch(res);
//         });
//     });
