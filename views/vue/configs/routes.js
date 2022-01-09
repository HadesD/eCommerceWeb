import Index from '../pages/Index';
import Category from '../pages/Category';
import Product from '../pages/Product';
import Search from '../pages/Search';
import Cart from '../pages/Cart';
import FAQ from '../pages/FAQ';

let routes = [
    {
        path: '',
        name: 'home',
        component: Index,
    },
    {
        path: '/cart',
        name: 'cart',
        component: Cart,
    },
    {
        path: '/search',
        name: 'search',
        component: Search,
    },
    {
        path: '/:category_slug',
        name: 'category',
        component: Category,
    },
    {
        path: '/:category_slug/:product_slug.:product_id',
        name: 'product',
        component: Product,
    },
    {
        path: '/:category_slug/:product_slug.:product_id',
        name: 'product',
        component: Product,
    },
    {
        path: '/faq/:faq_slug',
        name: 'faq',
        component: FAQ,
    },
];

export default routes;
