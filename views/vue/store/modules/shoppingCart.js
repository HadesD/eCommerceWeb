const localStorageKeyName = 'cartItems';

export default {
    state: () => ({
        cartItems: [
            /*
            {
                product: {},
                num: Number,
            },
            */
        ],
    }),

    actions: {
        fetchCartItems({ commit }) {
            try {
                let items = JSON.parse(localStorage.getItem(localStorageKeyName) || '[]') || [];
                commit('setCartItems', items);
            } catch (e) {
                localStorage.setItem(localStorageKeyName, '');
            }
        },

        appendCartItem({ commit, state }, item) {
            commit('addCartItem', item);

            localStorage.setItem(localStorageKeyName, JSON.stringify(state.cartItems));
        },

        removeCartItem({ commit, state }, item) {
            commit('removeCartItem', item);

            localStorage.setItem(localStorageKeyName, JSON.stringify(state.cartItems));
        },
    },

    mutations: {
        setCartItems(store, data) {
            store.cartItems = data;
        },

        addCartItem(store, item) {
            const exists = store.cartItems.find(elm => elm.product.id === item.product.id);
            if (!exists) {
                store.cartItems.push(item);
            } else {
                exists.num += item.num;
            }
        },

        /**
         * @param {*} store
         * @param {Object} item
         * @param {Number} num : Number of products being remove. 0: all
         */
        removeCartItem(store, item, num = 0) {
            for (const i in store.cartItems) {
                const curItem = store.cartItems[i];
                if (curItem.product.id === item.product.id) {
                    curItem.num -= num;
                    if ((num === 0) || (curItem.num <= 0)) {
                        store.cartItems.splice(i, 1);
                    }
                    break;
                }
            }
        },
    },

    getters: {
        getCartItems(store) {
            return store.cartItems;
        },
    },
}
