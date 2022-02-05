<template>
    <a-layout-header :style="{ padding: 0 }">
        <a-menu mode="horizontal" :selectedKeys="[$route.path]">
            <a-menu-item key="-" @click="sideBarCollapse">
                <MenuUnfoldOutlined v-if="sideBarCollapsed" />
                <MenuFoldOutlined v-else />
            </a-menu-item>
            <a-menu-item>
                <a href="/" target="_blank">Trang Chủ</a>
            </a-menu-item>
        </a-menu>
        <a-menu mode="horizontal" style="position: absolute; top: 0; right: 0;">
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
import { useRouter } from 'vue-router';

import {
    UserOutlined, DownOutlined, LogoutOutlined,
    MenuUnfoldOutlined, MenuFoldOutlined,
} from '@ant-design/icons-vue';

import User from '~/dashboard/utils/User';
import { message } from 'ant-design-vue';
import { showErrorRequestApi } from '../../../../../views/vue/helpers';

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
            const router = useRouter();

            const modal = this.$confirm({
                content: 'Chắc chắn muốn đăng xuất chứ?',
                onOk: () => {
                    return axios.post('/logout')
                        .then(res => {
                            User.clear();

                            modal.destroy();

                            router.push({ path: '/login' });

                            message.success('Đăng xuất thành công');
                        })
                        .catch(showErrorRequestApi)
                },
            });
        },
    },
}
</script>
