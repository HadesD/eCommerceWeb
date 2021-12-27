import helpers from '../../helpers';

export default {
    install(Vue, options) {
        Vue.prototype.$helpers = helpers;
    },
};
