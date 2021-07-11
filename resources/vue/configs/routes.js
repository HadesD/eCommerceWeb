import Main from '../layouts/Main';

let routes = [
    {
        path: '',
        component: Main,
        // redirect: '/',
        children: [
            {
                path: '/',
                component: Main,
            },
        ],
    },
];

export default routes;
