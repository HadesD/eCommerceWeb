import { createRouter, createWebHistory } from 'vue-router';

import Main from '../layouts/Main';

// const PagesRoute = () => import('./pages/index');

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

export default createRouter({
    history: createWebHistory('/'),
    routes,
});

