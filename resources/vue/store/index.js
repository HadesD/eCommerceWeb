import { createStore } from 'vuex';

import categories from './modules/categories';
import shoppingCart from './modules/shoppingCart';

export default createStore({
    modules: {
        categories,
        shoppingCart,
    },
});
