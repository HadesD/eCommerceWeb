import { createRouter, createWebHistory } from 'vue-router';

import Main from '../layouts/Main';

const RouteView = {
    // render: (h) => h('router-view')
    // template: `<router-view></router-view>`
    render: (h) => {
        console.log(h);
    }
};

let routes = [
    {
        path: '/',
        component: Main,
        // redirect: '/',
        children: [
            {
                path: '/',
                component: () => import('../pages/Index.vue'),
            },
            {
                path: '/products',
                component: () => import('../pages/products/Index.vue'),
            },
            {
                path: '/stocks',
                component: () => import('../pages/stocks/Index.vue'),
            },
            {
                path: '/orders',
                component: () => import('../pages/orders/Index.vue'),
            },
            {
                path: '/users',
                component: () => import('../pages/users/Index.vue'),
            },
            {
                path: '/transactions',
                component: () => import('../pages/transactions/Index.vue'),
            },
            {
                path: '/help',
                component: () => import('../pages/Help.vue'),
            },
        ],
    },
    {
        path: '/login',
        component: () => import('../pages/auth/Login.vue'),
    },
];

routes.push({
    path: '/(.*)',
    component: () => import('../pages/404.vue'),
});

export default createRouter({
    history: createWebHistory('/dashboard'),
    routes,
});
