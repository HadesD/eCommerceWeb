import RequestRepository from '../../utils/RequestRepository';

export default {
    state: () => ({
        categories: [],
    }),

    actions: {
        async fetchCategories({ commit }) {
            return RequestRepository.get('/categories')
                .then(res => {
                    commit('setCategories', res.data.data)
                });
        },
    },

    mutations: {
        setCategories(state, data) {
            state.categories = data;
        },
    },

    getters: {
        getCategories(state) {
            return state.categories;
        },
    },
};
