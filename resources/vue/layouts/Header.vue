<template>
    <a-layout-header>
        <div class="logo" />
        <a-menu
            theme="dark"
            mode="horizontal"
            :default-selected-keys="['2']"
            :style="{ lineHeight: '64px' }"
        >
            <a-menu-item key="1"> nav 1 </a-menu-item>
            <a-menu-item key="2"> nav 2 </a-menu-item>
            <a-menu-item key="3"> nav 3 </a-menu-item>
        </a-menu>
    </a-layout-header>
</template>
<script>
export default {

}
</script>
