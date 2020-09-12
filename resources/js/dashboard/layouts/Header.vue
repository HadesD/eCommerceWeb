<template>
  <a-layout-header :style="{ background: '#fff', padding: 0  }">
    <div class="logo"></div>
    <a-menu
      mode="horizontal"
      :selectedKeys="[$route.path]"
      :style="{ lineHeight: '64px' }"
      >
      <a-menu-item key="/">
        <router-link to="/">
          Trang quản lý
        </router-link>
      </a-menu-item>
    </a-menu>
  </a-layout-header>
</template>
<script>
export default {
}
</script>
