require('../bootstrap');

import Vue from 'vue'
import routes from './routes.js'
import Error404 from './pages/Error404.vue'

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
            const matchingView = routes[this.currentRoute];
            console.log(this.currentRoute);
            return matchingView ? matchingView : Error404;
        }
    },
    render (h) {
        return h(this.ViewComponent)
    }
});

window.addEventListener('popstate', () => {
    app.currentRoute = getRealRoute(window.location.pathname)
});

