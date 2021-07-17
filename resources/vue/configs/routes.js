import {
    defineAsyncComponent,
} from 'vue';

let routes = [
    {
        path: '',
        component: defineAsyncComponent(() => import('../pages/Index')),
        children: [
            {
                path: '/',
                // component: Main,
            },
        ],
    },
];

export default routes;
