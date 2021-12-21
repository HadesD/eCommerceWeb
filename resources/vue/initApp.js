import Antd from 'ant-design-vue';
import VueScreen from 'vue-screen';
import { createStore } from 'vuex';

import categories from './store/modules/categories';
import shoppingCart from './store/modules/shoppingCart';

const store = createStore({
    modules: {
        categories,
        shoppingCart,
    },
});

export default (app) => {
    app.use(store);
    app.use(Antd);
    app.use(VueScreen, 'bootstrap');
};
