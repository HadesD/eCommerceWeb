<template>
  <div>
    <a-row>
      <a-col :span="3">
        LOGO
      </a-col>
      <a-col :span="21">
        col-12
      </a-col>
    </a-row>

    <Breadcrumb></Breadcrumb>

    <slot></slot>
  </div>
</template>

<script>
import Breadcrumb from '../components/Breadcrumb.vue'
export default {
  components: {
    Breadcrumb,
  }
}
</script>
