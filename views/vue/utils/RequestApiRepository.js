import RequestRepository from './RequestRepository';
import RequestApi from './RequestApi';

import { showErrorRequestApi } from '~/helpers';

function get(url, cfg) {
    return new Promise(async (resolveFunc, rejectFunc) => {
        try {
            const res = await RequestRepository.get(url, cfg);

            resolveFunc(res.data);
        } catch (err) {
            showErrorRequestApi(err);

            rejectFunc(err);
        }
    });
};

function csrfToken() {
    return get('/csrf-token');
};

function post(url, data, cfg) {
    return new Promise(async (resolveFunc, rejectFunc) => {
        try {
            await csrfToken();
        } catch (error) {
            rejectFunc(error);
            return;
        }

        try {
            const res = await RequestRepository.post(url, data, cfg);

            resolveFunc(res.data);
        } catch (err) {
            showErrorRequestApi(err);

            rejectFunc(err);
        }
    });
};

export default {
    get, post
};
