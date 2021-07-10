<template>
    <div>{{ msg }}</div>
</template>

<script>
import RequestRepository from '../utils/RequestRepository';

export default {
    async data() {
        let e = 111;
        try {
            const a = await RequestRepository.get('/');
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
