import RequestRepository from './RequestRepository';

export default () => {
    return RequestRepository.get('/csrf-token');
};
