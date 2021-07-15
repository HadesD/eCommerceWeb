<template>
    <div>{{ msg }}</div>
</template>

<script>
import RequestRepository from '../utils/RequestRepository';

export default {
    data() {
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
