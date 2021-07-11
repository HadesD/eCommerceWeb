<template>
    <a-layout>
        <!-- <vue-progress-bar /> -->
        <SiderMenu :collapsed="sideBarCollapsed" :width="sideBarWidth" :collapsedWidth="sideBarCollapsedWidth"></SiderMenu>
        <a-layout>
            <Header :sideBarCollapsed="sideBarCollapsed" @onSetSidebarCollapsed="setSidebarCollapsed"></Header>
            <a-layout-content :style="{ margin: '24px 16px 0', overflow: 'initial' }">
                <!-- <Breadcrumb></Breadcrumb> -->
                <div :style="{ padding: '24px', background: '#fff', minHeight: 'calc(100vh - 160px)' }">
                    <router-view />
                </div>
            </a-layout-content>
            <a-layout-footer :style="{ textAlign: 'center' }">
                <div>From Dark.Hades with <HeartOutlined :style="{ color: 'hotpink' }" /></div>
            </a-layout-footer>
        </a-layout>
    </a-layout>
</template>

<script>
import {
    HeartOutlined,
} from '@ant-design/icons-vue';

import Breadcrumb from '../components/Breadcrumb';
import SiderMenu from '../layouts/SiderMenu';
import Header from '../layouts/Header';

export default {
    data() {
        return {
            sideBarCollapsed: false,
            sideBarWidth: 200,
        }
    },
    components: {
        Header, Breadcrumb, SiderMenu,
        HeartOutlined,
    },
    computed:{
        sideBarCollapsedWidth(){
            return this.isMobileSize ? 0 : 80;
        },
        isMobileSize(){
            const mq = this.$grid;
            return ['sm', 'md'].indexOf(mq.breakpoint) !== -1;
        },
    },
    mounted(){
        // this.$on('asyncComponentLoading', this.$Progress.start);
        // this.$on('asyncComponentLoaded', this.$Progress.finish);

        if (this.isMobileSize) {
            this.sideBarCollapsed = true;
        }
    },
    methods: {
        setSidebarCollapsed(sts){
            this.sideBarCollapsed = sts;
        },
    },
}
</script>
