const Status = {
    STS_SOLD: 1,
    STS_REFUND: 2,
};

export default Status;

const Config = {
    [Status.STS_SOLD]: {
        name: 'Đã bán',
        color: 'green',
    },
    [Status.STS_REFUND]: {
        name: 'Đã thu hồi',
        color: 'darkgrey',
    },
};

export { Config };
