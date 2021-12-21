export default {
    state: () => ({
        /**
         * []{product:{},num:Number}
         */
        cartItems: [],
    }),

    actions: {
        fetchCartProducts({ commit }) {
            let products = [];
            commit('setCartProducts', products);
        },

        appendCartProduct({ commit, store }, product) {
            commit('addCartProducts', product);
        },

        removeCartProduct({ commit }, product) {
            commit('removeCartProduct', product);
        },
    },

    mutations: {
        setCartProducts(store, data) {
            store.cartItems = data;
        },

        addCartProduct(store, product) {
            store.cartItems.push({
                product,
                num: 1,
            });
        },

        /**
         *
         * @param {*} store
         * @param {Object} product
         * @param {Number} num : Number of products being remove.
         *                       0: all
         */
        removeCartProduct(store, product, num = 0) {
            for (const i in store.cartItems) {
                const item = store.cartItems[i];
                if (item.product.id === product.id) {
                    item.num -= num;
                    if ((num === 0) || (item.num <= 0)) {
                        store.cartItems.splice(i, 1);
                    }
                    break;
                }
            }
        },
    },

    getters: {
        getCartProducts(store) {
            return store.cartItems;
        },
    },
}
