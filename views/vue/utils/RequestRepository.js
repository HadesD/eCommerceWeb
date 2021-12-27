import axios from 'axios';
import RequestApi from './RequestApi';

const ins = axios.create({
    ...RequestApi.defaults,
    baseURL: `${RequestApi.defaults.baseURL}`,
});

export default ins;
