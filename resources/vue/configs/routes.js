import Index from '../pages/Index';
import Category from '../pages/Category';
import Product from '../pages/Product';
import Search from '../pages/Search';

const RouteView = {
    template: `<router-view />`
};

let routes = [
    {
        path: '',
        name: 'home',
        component: Index,
    },
    {
        path: '/:slug',
        component: RouteView,
        children: [
            {
                path: '',
                name: 'category',
                component: Category,
            },
            {
                path: '/:slug',
                name: 'product',
                component: Product,
            },
        ],
    },
    {
        path: '/search',
        name: 'search',
        component: Search,
    },
];

export default routes;
