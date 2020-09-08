<template>
  <a-layout-header :style="{ background: '#fff', padding: 0  }">
    <div class="logo"></div>
    <a-menu
      mode="horizontal"
      :default-selected-keys="['1']"
      :style="{ lineHeight: '64px' }"
      >
      <a-menu-item key="1">
        <router-link to="/">
          Dashboard
        </router-link>
      </a-menu-item>
      <a-menu-item key="2">
        nav 2
      </a-menu-item>
      <a-menu-item key="3">
        nav 3
      </a-menu-item>
    </a-menu>
  </a-layout-header>
</template>
<script>
export default {
}
</script>
