import Vue from 'vue';
import Router from 'vue-router';

import Main from '../layouts/Main';

// const PagesRoute = () => import('./pages/index');

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
                component: Main,
            },
        ],
    },
];

export default new Router({
    // base: '/dashboard',
    mode: 'history',
    routes,
});

