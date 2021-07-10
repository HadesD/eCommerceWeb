<template>
    <div>{{ msg }}</div>
</template>

<script>
import RequestRepository from '../utils/RequestRepository';

export default {
    data() {
        return {
            msg: null,
        };
    },
    mounted() {
        RequestRepository.get('/sfsdsdf')
            .then(res => {
                this.msg = 113;
            })
            .catch(error => {
                this.msg = 111;
            });
    },
}
</script>
