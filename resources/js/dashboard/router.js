import Vue from 'vue'
import Router from 'vue-router'

Vue.use(Router);

const RouteView = {
    render: (h) => h('router-view')
}

let routes = [
    {
        path: '/',
        component: () => import('./layouts/Main.vue'),
        redirect: '/index',
        children: [
            {
                path: '/index',
                component: () => import('./pages/Index.vue'),
            },
            {
                path: '/products',
                redirect: '/products/index',
                component: RouteView,
                children: [
                    {
                        path: 'index',
                        component: () => import('./pages/products/Index.vue'),
                    },
                    {
                        path: 'new',
                        component: () => import('./pages/products/Edit.vue'),
                    },
                    {
                        path: ':id/edit',
                        component: () => import('./pages/products/Edit.vue'),
                    },
                ],
            },
            {
                path: '/stocks',
                redirect: '/stocks/index',
                component: RouteView,
                children: [
                    {
                        path: 'index',
                        component: () => import('./pages/stocks/Index.vue'),
                    },
                    {
                        path: 'new',
                        component: () => import('./pages/stocks/Edit.vue'),
                    },
                    {
                        path: ':id/edit',
                        component: () => import('./pages/stocks/Edit.vue'),
                    },
                ],
            },
            {
                path: '/orders',
                redirect: '/orders/index',
                component: RouteView,
                children: [
                    {
                        path: 'index',
                        component: () => import('./pages/orders/Index.vue'),
                    },
                    {
                        path: 'new',
                        component: () => import('./pages/orders/Edit.vue'),
                    },
                    {
                        path: ':id/edit',
                        component: () => import('./pages/orders/Edit.vue'),
                    },
                ],
            }
        ],
    },
    {
        path: '/login',
        component: () => import('./pages/auth/Login.vue'),
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

