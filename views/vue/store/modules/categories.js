import { showErrorRequestApi } from '../../helpers';
import RequestRepository from '../../utils/RequestRepository';

const type = {
    setLoadingCategories: 'setLoadingCategories',
    setCategories: 'setCategories',
};

export default {
    state: () => ({
        loadingCategories: false,
        categories: [],
    }),

    actions: {
        async fetchCategories({ commit }) {
            commit(type.setLoadingCategories, true);

            return RequestRepository.get('/categories')
                .then(res => {
                    commit(type.setCategories, res.data.data);
                })
                .catch(err => {
                    showErrorRequestApi(err);
                })
                .finally(() => (commit(type.setLoadingCategories, false)));
        },
    },

    mutations: {
        setLoadingCategories(state, loading) {
            state.loadingCategories = loading;
        },

        setCategories(state, data) {
            state.categories = data;

            state.loadingCategories = false;
        },
    },

    getters: {
        getLoadingCategories(state) {
            return state.loadingCategories;
        },
        getCategories(state) {
            return state.categories;
        },
    },
};
