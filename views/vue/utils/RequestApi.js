import axios from 'axios';
import RequestHttp from './RequestHttp';

const instance = axios.create({
    ...RequestHttp.defaults,
    baseURL: `${process.env.APP_URL||''}/api`,
});

export default instance;
