<template>
  <a-layout>
    <SiderMenu :collapsed="sideBarCollapsed" :width="sideBarWidth" :collapsedWidth="sideBarCollapsedWidth"></SiderMenu>
    <a-layout :style="{ marginLeft: (sideBarCollapsed ? sideBarCollapsedWidth : sideBarWidth) + 'px'  }">
      <Header :sideBarCollapsed="sideBarCollapsed" @onSetSidebarCollapsed="setSidebarCollapsed"></Header>
      <a-layout-content :style="{ margin: '24px 16px 0', overflow: 'initial' }">
        <!-- <Breadcrumb></Breadcrumb> -->
        <div :style="{ padding: '24px', background: '#fff', minHeight: '80vh' }">
          <router-view />
        </div>
      </a-layout-content>
      <a-layout-footer :style="{ textAlign: 'center' }">
        From Dark.Hades with &lt;3
      </a-layout-footer>
    </a-layout>
  </a-layout>
</template>

<script>
import Breadcrumb from '../components/Breadcrumb.vue'
import SiderMenu from '../layouts/SiderMenu.vue'
import Header from '../layouts/Header.vue'

export default {
  data() {
    return {
      sideBarCollapsed: false,
      sideBarWidth: 200,
      sideBarCollapsedWidth: 0,
    }
  },
  components: {
    Header, Breadcrumb, SiderMenu
  },
  methods: {
    setSidebarCollapsed(sts){
      this.sideBarCollapsed = sts;
    },
  },
}
</script>
