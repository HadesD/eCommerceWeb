import Antd from 'ant-design-vue';
import VueScreen from 'vue-screen';

import store from './store';

export default (app) => {
    app.use(store);
    app.use(Antd);
    app.use(VueScreen, 'bootstrap');
};
