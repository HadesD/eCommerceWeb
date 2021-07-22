<template>
    <a-layout>
        <Header />
        <a-layout-content style="margin: 25px 0;">
            <div :style="{ background: '#fff', padding: '24px', }">
                <router-view></router-view>
            </div>
        </a-layout-content>
        <Footer />
    </a-layout>
</template>

<script>
import Header from './Header';
import Footer from './Footer';

import RequestRepository from '../utils/RequestRepository';

export default {
    components: {
        Header, Footer,
    },
}
</script>
