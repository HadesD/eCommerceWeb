<template>
  <div>
  </div>
</template>
<script>
export default {
  data(){
    return {

    }
  },
  mounted(){
    axios.get('/sanctum/csrf-cookie').then(response => {
      // Login...
    });
  },
}
</script>
