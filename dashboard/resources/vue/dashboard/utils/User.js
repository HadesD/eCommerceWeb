import { hasPermission } from '../configs/UserRole';

const k = 'user_info';

export default {
    info() {
        const user_info = localStorage.getItem(k);

        let info = {
            hasPermission: (role) => hasPermission(this.info().role, role),
        };

        if (user_info) {
            try {
                info = {
                    ...info,
                    ...JSON.parse(user_info),
                };
            }
            catch (e) {
                this.clear();
            }
        }

        return info;
    },
    clear() {
        localStorage.removeItem(k);
    },
    setInfo(info) {
        localStorage.setItem(k, JSON.stringify(info));
    },
};
