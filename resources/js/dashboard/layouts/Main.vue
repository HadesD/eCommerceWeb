<template>
  <a-layout>
    <Slider />
    <a-layout :style="{ marginLeft: '200px'  }">
      <Header />
        <a-layout-content :style="{ margin: '24px 16px 0', overflow: 'initial' }">
          <!-- <Breadcrumb></Breadcrumb> -->
          <div :style="{ padding: '24px', background: '#fff', minHeight: '80vh' }">
            <slot></slot>
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
import Slider from '../layouts/Slider.vue'
import Header from '../layouts/Header.vue'

export default {
  components: {
    Header, Breadcrumb, Slider
  },
}
</script>
