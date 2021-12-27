const Status = {
    STS_PROCESSING: 0,
    STS_CANCELED: 10,
    STS_PAID: 50,
    STS_PAYING: 51,
    STS_SHIPPING: 100,
    STS_COMPLETED: 200,
};

export default Status;

const Config = {
    [Status.STS_PROCESSING]: {
        name: 'Đang xử lí',
        color: '#F50',
    },
    [Status.STS_CANCELED]: {
        name: 'Đã hủy bỏ',
        color: 'darkgrey',
    },
    [Status.STS_PAYING]: {
        name: 'Đang thanh toán',
        color: 'red',
    },
    [Status.STS_PAID]: {
        name: 'Đã thanh toán',
        color: 'green',
    },
    [Status.STS_SHIPPING]: {
        name: 'Đang ship',
        color: 'orange',
    },
    [Status.STS_COMPLETED]: {
        name: 'Hoàn tất',
        color: 'blue',
    },
};

export { Config };
