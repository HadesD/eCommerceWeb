import Vue from 'vue'
import Router from 'vue-router'

const RouteView = {
  render: (h) => h('router-view')
}

Vue.use(Router);

let routes = [
    {
        path: '/',
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
                component: () => import('./pages/stocks/New.vue'),
            },
            {
                path: 'edit',
                component: () => import('./pages/stocks/Edit.vue'),
            },
        ],
    },
    {
        path: '/categories',
        redirect: '/categories/index',
        component: RouteView,
        children: [
            {
                path: 'index',
                component: () => import('./pages/categories/Index.vue'),
            },
        ],
    }
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

