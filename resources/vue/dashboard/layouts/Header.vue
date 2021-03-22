<template>
  <a-layout-header :style="{ background: '#fff', padding: 0 }">
    <a-menu
      mode="horizontal"
      :selectedKeys="[$route.path]"
      :style="{ lineHeight: '64px' }"
      >
      <a-menu-item key="-" @click="sideBarCollapse">
        <a-icon :type="sideBarCollapsed ? 'menu-unfold' : 'menu-fold'" style="font-size: 25px;vertical-align: middle;" />
      </a-menu-item>
      <a-menu-item>
          <a href="/" target="_blank">Trang Chủ</a>
      </a-menu-item>
      <a-menu-item key="--" style="float:right;">
        <a-dropdown>
          <a @click="e => e.preventDefault()">
            <a-icon type="user"></a-icon> {{ userName }} <a-icon type="down"></a-icon>
          </a>
          <a-menu slot="overlay">
            <a-menu-item>
              <a @click="logout">
                <a-icon type="logout"></a-icon> Đăng xuất
              </a>
            </a-menu-item>
          </a-menu>
        </a-dropdown>
      </a-menu-item>
    </a-menu>
  </a-layout-header>
</template>
<script>
import User from '../utils/User';

export default {
  props: {
    sideBarCollapsed: {
      type: Boolean,
    }
  },
  data() {
    return {

    }
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
      const self = this;
      const modal = this.$confirm({
        title: 'Chú ý',
        content: 'Chắc chắn muốn đăng xuất chứ?',
        onOk() {
          return axios.post('/logout')
            .then(res => {
              User.clear();

              self.$router.push({ path: '/login' });

              self.$message.success('Đăng xuất thành công');
            })
            .catch(res => {
              console.log(res);
              self.$message.error('Đăng xuất thất bại');
            })
            .finally(res => {
              modal.destroy();
            });
        },
      });
    },
  },
}
</script>
