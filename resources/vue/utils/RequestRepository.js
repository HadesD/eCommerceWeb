import RequestApi from './RequestApi';

const repositoryInstance = RequestApi.create({
    ...RequestApi.defaults,
    baseURL: '/api',
});

export default repositoryInstance;
