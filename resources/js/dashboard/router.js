import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router);

export default new Router({
    base: '/dashboard',
    mode: 'history',
    routes: [
        {
            path: '/',
            component: () => import('./pages/Index.vue'),
        },

        // Must be the last end
        {
            path: '*',
            component: () => import('./pages/404.vue'),
        }
    ],
})

