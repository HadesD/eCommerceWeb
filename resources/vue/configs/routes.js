import Index from '../pages/Index';
import Category from '../pages/Category';
import Product from '../pages/Product';

let routes = [
    {
        path: '',
        name: 'home',
        component: Index,
    },
    {
        path: '/:category_slug',
        name: 'category',
        component: Category,
        children: [
            {
                path: '/:product_slug',
                name: 'product',
                component: Product,
            },
        ],
    },
];

export default routes;
