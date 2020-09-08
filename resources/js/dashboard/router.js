import Vue from 'vue'
import Router from 'vue-router'

import Index from './pages/Index.vue'
import Error404 from './pages/Error404.vue'

Vue.use(Router);

export default new Router({
    base: '/dashboard',
    mode: 'history',
    routes: [
        {
            path: '/',
            component: Index,
        },
    ],
})

