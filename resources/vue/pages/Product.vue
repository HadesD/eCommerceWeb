<template>
    product
</template>
<script>
import { useRoute, useRouter } from 'vue-router'
export default {
    setup() {
        console.log(useRouter());
        console.log(useRoute());
    },
}
</script>
