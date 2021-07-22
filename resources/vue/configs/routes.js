import Index from '../pages/Index';
import Category from '../pages/Category';
import Product from '../pages/Product';

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
        path: '/:category_slug',
        component: RouteView,
        children: [
            {
                path: '',
                name: 'category',
                component: Category,
            },
            {
                path: ':product_slug',
                name: 'product',
                component: Product,
            },
        ],
    },
];

export default routes;
