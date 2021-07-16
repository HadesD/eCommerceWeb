<template>
    <a-layout>
        <Header />
    </a-layout>
</template>

<script>
import Header from './Header';

import RequestRepository from '../utils/RequestRepository';

export default {
    components: {
        Header,
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
