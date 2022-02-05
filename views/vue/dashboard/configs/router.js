import { createRouter, createWebHistory } from 'vue-router';

import Main from '~/dashboard/layouts/Main.vue';

let routes = [
    {
        path: '/',
        component: Main,
        children: [
            {
                path: '/',
                component: (() => import('~/dashboard/pages/Index.vue')),
            },
            {
                path: '/products',
                component: (() => import('~/dashboard/pages/products/Index.vue')),
            },
            {
                path: '/stocks',
                component: (() => import('~/dashboard/pages/stocks/Index.vue')),
            },
            {
                path: '/orders',
                component: (() => import('~/dashboard/pages/orders/Index.vue')),
            },
            {
                path: '/orders/new',
                component: (() => import('~/dashboard/pages/orders/Edit.vue')),
            },
            {
                path: '/users',
                component: (() => import('~/dashboard/pages/users/Index.vue')),
            },
            {
                path: '/transactions',
                component: (() => import('~/dashboard/pages/transactions/Index.vue')),
            },
            {
                path: '/help',
                component: (() => import('~/dashboard/pages/Help.vue')),
            },
        ],
    },
    {
        path: '/login',
        component: (() => import('~/dashboard/pages/auth/Login.vue')),
    },
];

routes.push({
    path: '/(.*)',
    component: (() => import('~/dashboard/pages/404.vue')),
});

export default createRouter({
    history: createWebHistory('/'),
    routes,
});
