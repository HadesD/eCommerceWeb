import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router);

let routes = [
    {
        path: '/',
        component: () => import('./pages/Index.vue'),
    },
    {
        path: '/products',
        component: () => import('./pages/products/Index.vue'),
        children: [
            {
                path: '/edit',
                component: () => import('./pages/products/Edit.vue'),
            },
        ]
    },
];

routes.push({
    path: '*',
    component: () => import('./pages/404.vue'),
});

export default new Router({
    base: '/dashboard',
    mode: 'history',
    routes,
})

