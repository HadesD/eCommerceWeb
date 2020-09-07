require('../bootstrap');

import Vue from 'vue'
import routes from './routes.js'
import Error404 from './pages/Error404.vue'
import Antd from 'ant-design-vue';

Vue.use(Antd);

const getRealRoute = (path) => {
    return path.substring('/dashboard'.length);
};

const app = new Vue({
    el: '#app',
    data: {
        currentRoute: getRealRoute(window.location.pathname)
    },
    computed: {
        ViewComponent () {
            return routes[this.currentRoute] || Error404;
        }
    },
    render (h) {
        return h(this.ViewComponent);
    }
});

window.addEventListener('popstate', () => {
    app.currentRoute = getRealRoute(window.location.pathname)
});

