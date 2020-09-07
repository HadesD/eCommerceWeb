<template>
  <Main>
    <a-result status="404" title="404" sub-title="Sorry, the page you visited does not exist.">
      <template #extra>
        <a-button type="primary">
          Back Home
        </a-button>
      </template>
    </a-result>
  </Main>
</template>

<script>
import Main from '../layouts/Main.vue'
export default {
  components: {
    Main,
  }
}
</script>
