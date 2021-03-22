export default {
    install(Vue, options) {
        Vue.prototype.$user = (() => {
            const k = 'user_info';

            return {
                info() {
                    const user_info = localStorage.getItem(k);
                    if (user_info) {
                        try {
                            return JSON.parse(user_info);
                        }
                        catch (e) {
                            this.clear();
                        }
                    }

                    return {};
                },
                clear() {
                    localStorage.removeItem(k);
                },
                setInfo(info) {
                    localStorage.setItem(k, JSON.stringify(info));
                },
            };
        })();
    },
}

