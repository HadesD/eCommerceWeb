const Method = {
    PM_ONCE: 1,
    PM_INSTALMENT: 2, // Tra gop
};

export default Method;

const Config = {
    [Method.PM_ONCE]: {
        name: 'Trả thẳng 100%',
        color: 'green',
    },
    [Method.PM_INSTALMENT]: {
        name: 'Trả góp',
        color: 'red',
    },
};

export { Config };
