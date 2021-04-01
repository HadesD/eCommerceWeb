<template>
  <a-layout-sider v-model="collapsed" id="slider_sidebar" :width="width" :collapsedWidth="collapsedWidth">
    <a-menu
      theme="dark"
      mode="inline"
      :defaultOpenKeys="['/products', '/stocks', '/orders', '/users']"
      :selectedKeys="[$route.path]"
      :style="{ height: '100%', borderRight: 0 }"
      >
      <a-menu-item key="-" style="padding:0;">
        <div class="logo">
          <router-link to="/">
            <img src="/favicon.ico" /><span v-if="!collapsed" style="color:#FFF;font-size:25px;font-weight:bold;vertical-align: bottom;">inPhone.vn</span>
          </router-link>
        </div>
      </a-menu-item>
      <a-sub-menu key="/products">
        <span slot="title">
          <a-icon type="shopping-cart" /><span>Sản phẩm</span>
        </span>
        <a-menu-item key="/products/">
          <router-link to="/products/">Danh sách</router-link>
        </a-menu-item>
        <a-menu-item key="/products/new">
          <router-link to="/products/new">Đăng bán</router-link>
        </a-menu-item>
      </a-sub-menu>
      <a-sub-menu key="/orders">
        <span slot="title">
          <a-icon type="shopping-cart" /><span>Hoá Đơn</span>
        </span>
        <a-menu-item key="/orders/">
          <router-link to="/orders/">Danh sách</router-link>
        </a-menu-item>
        <a-menu-item key="/orders/new">
          <router-link to="/orders/new">Tạo đơn</router-link>
        </a-menu-item>
      </a-sub-menu>
      <a-sub-menu key="/stocks">
        <span slot="title">
          <a-icon type="bank" /><span>Kho hàng</span>
        </span>
        <a-menu-item key="/stocks/">
          <router-link to="/stocks/">Danh sách</router-link>
        </a-menu-item>
        <a-menu-item key="/stocks/new">
          <router-link to="/stocks/new">Nhập kho</router-link>
        </a-menu-item>
      </a-sub-menu>
      <a-sub-menu key="/users">
        <span slot="title">
          <a-icon type="user" /><span>Khách hàng</span>
        </span>
        <a-menu-item key="/users/">
          <router-link to="/users/">Danh sách</router-link>
        </a-menu-item>
      </a-sub-menu>
    </a-menu>
  </a-layout-sider>
</template>
<script>
export default {
  props: {
    width: {
      type: Number,
    },
    collapsedWidth: {
      type: Number,
    },
    collapsed: {
      type: Boolean,
      default: false,
    }
  },
  data() {
    return {
    }
  },
}
</script>
