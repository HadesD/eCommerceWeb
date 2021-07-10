import Vue from 'vue';
import Router from 'vue-router';

import Main from '../layouts/Main';

Vue.use(Router);

const RouteView = {
    render: (h) => h('router-view')
};

let routes = [
    {
        path: '',
        component: Main,
        redirect: '/',
        children: [
            {
                path: '/',
                component: () => import('../pages/Index.vue'),
            },
            {
                path: '/products',
                component: RouteView,
                children: [
                    {
                        path: '/',
                        component: () => import('../pages/products/Index.vue'),
                    },
                    {
                        path: 'new',
                        component: () => import('../pages/products/Edit.vue'),
                    },
                ],
            },
            {
                path: '/stocks',
                component: RouteView,
                children: [
                    {
                        path: '/',
                        component: () => import('../pages/stocks/Index.vue'),
                    },
                    {
                        path: 'new',
                        component: () => import('../pages/stocks/Edit.vue'),
                    },
                ],
            },
            {
                path: '/orders',
                component: RouteView,
                children: [
                    {
                        path: '/',
                        component: () => import('../pages/orders/Index.vue'),
                    },
                    {
                        path: 'new',
                        component: () => import('../pages/orders/Edit.vue'),
                    },
                ],
            },
            {
                path: '/users',
                component: RouteView,
                children: [
                    {
                        path: '/',
                        component: () => import('../pages/users/Index.vue'),
                    },
                ],
            },
            {
                path: '/transactions',
                component: RouteView,
                children: [
                    {
                        path: '/',
                        component: () => import('../pages/transactions/Index.vue'),
                    },
                ],
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
    path: '*',
    component: () => import('../pages/404.vue'),
});

export default new Router({
    base: '/dashboard',
    mode: 'history',
    routes,
});
