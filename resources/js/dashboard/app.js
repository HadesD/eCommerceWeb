require('../bootstrap');

import Vue from 'vue'
import routes from './routes.js'
import Error404 from './pages/Error404.vue'

const app = new Vue({
    el: '#app',
    data: {
        currentRoute: window.location.pathname
    },
    computed: {
        ViewComponent () {
            const matchingView = routes[this.currentRoute];
            return matchingView ? matchingView : Error404;
        }
    },
    render (h) {
        return h(this.ViewComponent)
    }
});

window.addEventListener('popstate', () => {
    app.currentRoute = window.location.pathname
})

