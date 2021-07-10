import RequestApi from './RequestApi';

export default RequestApi.create({
    baseURL: `${process.env.APP_URL||''}/api/dashboard`,
});
