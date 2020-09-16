<template>
  <div>

  </div>
</template>
<script>
export default {
  data(){
    return {
      csrf: '',
      formData: {
        email: '',
        password: '',
        remember: true,
      },
    }
  },
  mounted(){
    axios.get('/sanctum/csrf-cookie').then(response => {
      console.log(response);
    });
  },
}
</script>
