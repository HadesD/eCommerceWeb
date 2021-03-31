const Status = {
    STS_DRAFT: 0,
    STS_SELLING: 1,
    STS_SOLDOUT: 2,
};

export default Status;

const Config = {
    [Status.STS_DRAFT]: {
        name: 'Bản nháp',
        color: '#F50',
    },
    [Status.STS_SELLING]: {
        name: 'Đang bán',
        color: 'green',
    },
    [Status.STS_SOLDOUT]: {
        name: 'Hết hàng',
        color: 'darkgrey',
    },
};

export { Config };

