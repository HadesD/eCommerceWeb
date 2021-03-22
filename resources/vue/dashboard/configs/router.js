import Vue from 'vue';
import Router from 'vue-router';

// const PagesRoute = () => import('./pages/index');

Vue.use(Router);

const RouteView = {
    render: (h) => h('router-view')
};

let routes = [
    {
        path: '',
        component: () => import('../layouts/Main.vue'),
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
                    {
                        path: ':id/edit',
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
                    {
                        path: ':id/edit',
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
                    {
                        path: ':id/edit',
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
                    {
                        path: 'new',
                        component: () => import('../pages/users/Edit.vue'),
                    },
                    {
                        path: ':id/edit',
                        component: () => import('../pages/users/Edit.vue'),
                    },
                ],
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
