const Role = {
    ROLE_USER_BLOCKED: 0,
    ROLE_USER_UNACTIVE: 10,
    ROLE_USER_NORMAL: 50,
    ROLE_ADMIN_MANAGER: 100,
    ROLE_ADMIN_MASTER: 200,
};

export default Role;

const Config = {
    [Role.ROLE_USER_BLOCKED]: {
        name: 'Đã bị khoá',
        color: 'black',
    },
    [Role.ROLE_USER_UNACTIVE]: {
        name: 'Chưa kích hoạt',
        color: 'yellow',
    },
    [Role.ROLE_USER_NORMAL]: {
        name: 'Người dùng',
        color: 'blue',
    },
    [Role.ROLE_ADMIN_MANAGER]: {
        name: 'Quản trị viên',
        color: 'red',
    },
    [Role.ROLE_ADMIN_MASTER]: {
        name: 'Một vị Thần',
        color: 'grey',
    },
};

export { Config };
