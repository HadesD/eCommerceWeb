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
            let items = [];
            commit('setCartItems', items);
        },

        appendCartItem({ commit, store }, item) {
            commit('addCartItem', item);
        },

        removeCartItem({ commit }, item) {
            commit('removeCartItem', item);
        },
    },

    mutations: {
        setCartItems(store, data) {
            store.cartItems = data;
        },

        addCartItem(store, item) {
            const exists = store.cartItems.find(elm => elm.product.id === item.product.id);
            console.log(exists, item.product, item.product.id);
            if (!exists) {
                store.cartItems.push(item);
            } else {
                exists.num += item.num;
            }
        },

        /**
         *
         * @param {*} store
         * @param {Object} item
         * @param {Number} num : Number of products being remove.
         *                       0: all
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
