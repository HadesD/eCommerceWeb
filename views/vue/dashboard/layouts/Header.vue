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
import { message, Modal } from 'ant-design-vue';
import { showErrorRequestApi } from '../../../../dashboard/resources/vue/helpers';
import RequestApiRepository from '~/utils/RequestApiRepository';
import { computed } from '@vue/runtime-core';

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

    setup() {
        const router = useRouter();

        const logout = async () => {
            const modal = Modal.confirm({
                content: 'Chắc chắn muốn đăng xuất chứ?',
                onOk: () => {
                    return RequestApiRepository.post('/logout')
                        .then(res => {
                            User.clear();

                            modal.destroy();

                            router.push({ name: 'login' });

                            message.success('Đăng xuất thành công');
                        })
                        .catch(showErrorRequestApi)
                },
            });
        };

        return {
            userName: computed(() => (User.info().name || 'Admin')),
            logout,

            sideBarCollapse() {
                this.$emit('collapse', !this.sideBarCollapsed);
            },
        };
    },
}
</script>
