<template>
    <Main>
        <router-view></router-view>
    </Main>
</template>
<script>
import Main from './layouts/Main.vue'
export default {
    components: {
        Main
    }
}
</script>
