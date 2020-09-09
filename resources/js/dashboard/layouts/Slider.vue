<template>
  <a-layout-sider v-model="collapsed" collapsible id="slider_sidebar">
    <div class="logo" v-lazy-container="{ selector: 'img' }">
      <img data-src="/images/logo.png" />
    </div>
    <a-menu
      theme="dark"
      mode="inline"
      :default-selected-keys="['1']"
      :default-open-keys="['sub1']"
      :style="{ height: '100%', borderRight: 0, backgroundColor: '#9800ab' }"
      >
      <a-sub-menu key="sub1">
        <span slot="title"><a-icon type="user" /><span>subnav 1</span></span>
        <a-menu-item key="1">
          option1
        </a-menu-item>
        <a-menu-item key="2">
          option2
        </a-menu-item>
        <a-menu-item key="3">
          option3
        </a-menu-item>
        <a-menu-item key="4">
          option4
        </a-menu-item>
      </a-sub-menu>
      <a-sub-menu key="sub2">
        <span slot="title"><a-icon type="user" /><span>subnav 1</span></span>
        <a-menu-item key="5">
          option5
        </a-menu-item>
        <a-menu-item key="6">
          option6
        </a-menu-item>
        <a-menu-item key="7">
          option7
        </a-menu-item>
        <a-menu-item key="8">
          option8
        </a-menu-item>
      </a-sub-menu>
      <a-sub-menu key="sub3">
        <span slot="title"><a-icon type="user" /><span>subnav 1</span></span>
        <a-menu-item key="9">
          option9
        </a-menu-item>
        <a-menu-item key="10">
          option10
        </a-menu-item>
        <a-menu-item key="11">
          option11
        </a-menu-item>
        <a-menu-item key="12">
          option12
        </a-menu-item>
      </a-sub-menu>
    </a-menu>
  </a-layout-sider>
</template>
<script>
export default {
  data() {
    return {
      collapsed: false,
    };
  },
}
</script>
<style>
#slider_sidebar {
  overflow: auto;
  height: 100vh;
  position: fixed;
  left: 0;
  top: 0;
  -webkit-box-shadow: 2px 0 6px rgba(0,21,41,.35);
  box-shadow: 2px 0 6px rgba(0,21,41,.35);
}

#slider_sidebar .logo {
  padding: 5px;
  text-align: center;
}

#slider_sidebar .logo img{
  max-width: 100%;
}

#slider_sidebar .ant-menu-dark .ant-menu-inline.ant-menu-sub{
  background-color: #680075;
}
</style>
