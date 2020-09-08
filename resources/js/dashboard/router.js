import Vue from 'vue'
import Router from 'vue-router'

import Index from './pages/Index.vue'

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

