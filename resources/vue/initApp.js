import Antd from 'ant-design-vue';
import VueScreen from 'vue-screen';
import { createMetaManager } from 'vue-meta';
import { plugin as vueMetaPlugin } from 'vue-meta'

import store from './store';

export default (app, config) => {
    app
        .use(createMetaManager(config.ssr))
        .use(vueMetaPlugin)
        .use(store)
        .use(Antd)
        .use(VueScreen, 'bootstrap');
};
