import { createRouter, createWebHistory } from 'vue-router';

import Main from '~/dashboard/layouts/Main.vue';

import ProductIndex from '~/dashboard/pages/products/Index.vue';
import StockIndex from '~/dashboard/pages/stocks/Index.vue';
import OrderIndex from '~/dashboard/pages/orders/Index.vue';
import OrderEdit from '~/dashboard/pages/orders/Edit.vue';
import UserIndex from '~/dashboard/pages/users/Index.vue';
import TransactionIndex from '~/dashboard/pages/transactions/Index.vue';

let routes = [
    {
        path: '/',
        component: Main,
        children: [
            {
                name: 'top',
                path: '/',
                component: (() => import('~/dashboard/pages/Index.vue')),
            },
            {
                name: 'products',
                path: '/products',
                component: ProductIndex,
            },
            {
                name: 'stocks',
                path: '/stocks',
                component: StockIndex,
            },
            {
                name: 'orders',
                path: '/orders',
                component: OrderIndex,
            },
            {
                name: 'order_create',
                path: '/orders/new',
                component: OrderEdit,
            },
            {
                name: 'users',
                path: '/users',
                component: UserIndex,
            },
            {
                name: 'transactions',
                path: '/transactions',
                component: TransactionIndex,
            },
            {
                name: 'help',
                path: '/help',
                component: (() => import('~/dashboard/pages/Help.vue')),
            },
        ],
    },
    {
        name: 'login',
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
