import app from './app';
import renderVueComponentToString from 'vue-server-renderer/basic';


// renderVueComponentToString(app, (err, res) => {
//     if (err) throw new Error(err);
//     dispatch(res);
// });

new Promise((resolve, reject) => {
    app.$router.push(context.url);
    app.$router.onReady(() => {
        const matchedComponents = app.$router.getMatchedComponents();
        if (!matchedComponents.length) {
            return reject({ code: 404 });
        }
        resolve(app);
    }, reject);
})
    .then(app => {
        renderVueComponentToString(app, (err, res) => {
            if (err) throw new Error(err);
            dispatch(res);
        });
    })
    .catch((err) => {
        dispatch(err);
    });
