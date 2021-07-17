<template>
    <div id="app">
        <a-layout>
            <Header />
            <a-layout-content style="padding: 0 50px;margin-bottom: 25px;">
                <a-breadcrumb style="margin: 16px 0">
                    <a-breadcrumb-item>Home</a-breadcrumb-item>
                    <a-breadcrumb-item>List</a-breadcrumb-item>
                    <a-breadcrumb-item>App</a-breadcrumb-item>
                </a-breadcrumb>
                <div :style="{ background: '#fff', padding: '24px', }">
                    <router-view></router-view>
                </div>
            </a-layout-content>
            <Footer />
        </a-layout>
    </div>
</template>

<script>
import Header from './Header';
import Footer from './Footer';

import RequestRepository from '../utils/RequestRepository';

export default {
    components: {
        Header, Footer,
    },

    setup() {
        let e = 11;
        try {
            const a = RequestRepository.get('/');
        } catch (e) {
            e = e || 1;
        }

        return {
            msg: e,
        };
    },

    async mounted() {
    },
}
</script>
