<template>
  <a-layout-sider v-model="collapsed" collapsible id="slider_sidebar">
    <div class="logo">
      <router-link to="/">
        <img src="/favicon.ico" /><span v-if="!collapsed" style="color:#FFF;font-size:25px;font-weight:bold;vertical-align: bottom;">inPhone.vn</span>
      </router-link>
    </div>
    <a-menu
      theme="dark"
      mode="inline"
      :default-open-keys="['/products','/stocks']"
      :default-selected-keys="['/products/index']"
      :style="{ height: '100%', borderRight: 0, backgroundColor: '#9800ab' }"
      >
      <a-sub-menu key="/products">
        <span slot="title">
          <a-icon type="shopping-cart" /><span>Sản phẩm</span>
        </span>
        <a-menu-item key="/products/index">
          <router-link to="/products/index">
            Danh sách
          </router-link>
        </a-menu-item>
        <a-menu-item key="/products/new">
          <router-link to="/products/new">
            Đăng bán
          </router-link>
        </a-menu-item>
      </a-sub-menu>
      <a-sub-menu key="/stocks">
        <span slot="title">
          <a-icon type="bank" /><span>Kho hàng</span>
        </span>
        <a-menu-item key="/stocks/index">
          <router-link to="/stocks/index">
            Danh sách
          </router-link>
        </a-menu-item>
        <a-menu-item key="/stocks/new">
          <router-link to="/stocks/new">
            Nhập hàng
          </router-link>
        </a-menu-item>
        <a-menu-item key="/stocks/edit">
          Xuất kho
        </a-menu-item>
      </a-sub-menu>
      <!-- <a&#45;menu&#45;item key="/categories/index"> -->
      <!--   <router&#45;link to="/categories/index"> -->
      <!--     <a&#45;icon type="bars" /><span>Chuyên mục</span> -->
      <!--   </router&#45;link> -->
      <!-- </a&#45;menu&#45;item> -->
    </a-menu>
  </a-layout-sider>
</template>
<script>
export default {
  data() {
    return {
      collapsed: false,
    }
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
