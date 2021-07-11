<template>
    <a-layout-header :style="{ padding: 0 }">
        <a-menu
            mode="horizontal"
            :selectedKeys="[$route.path]"
            :style="{ lineHeight: '64px' }"
        >
            <a-menu-item key="-" @click="sideBarCollapse">
                <MenuUnfoldOutlined v-if="sideBarCollapsed" />
                <MenuFoldOutlined v-else />
            </a-menu-item>
            <a-menu-item>
                <a href="/" target="_blank">Trang Chủ</a>
            </a-menu-item>
            <a-menu-item key="--">
                <a-dropdown>
                    <a @click="e => e.preventDefault()">
                        <UserOutlined /> {{ userName }}<DownOutlined />
                    </a>
                    <template #overlay>
                        <a-menu>
                            <a-menu-item>
                                <a @click="logout">
                                    <LogoutOutlined /> Đăng xuất
                                </a>
                            </a-menu-item>
                        </a-menu>
                    </template>
                </a-dropdown>
            </a-menu-item>
        </a-menu>
    </a-layout-header>
</template>
<script>
import User from '../utils/User';

import {
    UserOutlined, DownOutlined, LogoutOutlined,
    MenuUnfoldOutlined, MenuFoldOutlined,
} from '@ant-design/icons-vue';

export default {
    components: {
        UserOutlined, DownOutlined, LogoutOutlined,
        MenuUnfoldOutlined, MenuFoldOutlined,
    },
    props: {
        sideBarCollapsed: {
            type: Boolean,
        }
    },
    data() {
        return {

        };
    },
    computed: {
        userName(){
            return User.info().name || 'Admin'
        },
    },
    methods: {
        sideBarCollapse(){
            this.$emit('onSetSidebarCollapsed', !this.sideBarCollapsed);
        },
        logout(){
            const modal = this.$confirm({
                content: 'Chắc chắn muốn đăng xuất chứ?',
                onOk: () => {
                    return axios.post('/logout')
                        .then(res => {
                            User.clear();

                            modal.destroy();

                            this.$router.push({ path: '/login' });

                            this.$message.success('Đăng xuất thành công');
                        })
                        .catch(err => {
                            if (err.response && err.response.data && err.response.data.message) {
                                this.$message.error(err.response.data.message);
                                return;
                            }

                            this.$message.error(err.message || 'Thất bại');
                        })
                },
            });
        },
    },
}
</script>
