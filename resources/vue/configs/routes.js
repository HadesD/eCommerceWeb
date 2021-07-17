import Index from '../pages/Index';

let routes = [
    {
        path: '',
        name: 'home',
        component: Index,
        children: [
            {
                path: '/',
                // component: Main,
            },
        ],
    },
];

export default routes;
