const Status = {
    STS_PENDING: 0,
    STS_COMPLETED: 1,
    STS_CANCELED: -1,
};

export default Status;

const Config = {
    [Status.STS_PENDING]: {
        name: 'Đang xử lí',
        color: '#F50',
    },
    [Status.STS_CANCELED]: {
        name: 'Đã hủy bỏ',
        color: 'darkgrey',
    },
    [Status.STS_COMPLETED]: {
        name: 'Hoàn tất',
        color: 'green',
    },
};

export { Config };
