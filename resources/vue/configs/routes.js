import Index from '../pages/Index';
import Category from '../pages/Category';
import Product from '../pages/Product';
import Search from '../pages/Search';

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
    },
    {
        path: '/:category_slug/:product_slug',
        name: 'product',
        component: Product,
    },
    {
        path: '/search',
        name: 'search',
        component: Search,
    },
];

export default routes;
