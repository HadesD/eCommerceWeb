<template>
  <div>
    <h2>
      Kho hàng
      <a-tooltip title="Làm mới">
        <a-button type="primary" icon="reload" :loading="false" @click="() => {}" />
      </a-tooltip>
      <router-link to="/stocks/new">
        <a-tooltip title="Nhập kho">
          <a-button type="primary" icon="plus" style="float:right;" />
        </a-tooltip>
      </router-link>
    </h2>
    <a-table :columns="columns" :data-source="data">
      <a slot="name" slot-scope="text">{{ text }}</a>
    </a-table>
  </div>
</template>

<script>
import axios from 'axios';

export default {
  data() {
    return {
      columns: [
        {
          title: 'Name',
          dataIndex: 'name',
          key: 'name',
          scopedSlots: { customRender: 'name' },
        },
        {
          title: 'Age',
          dataIndex: 'age',
          key: 'age',
          width: 80,
        },
        {
          title: 'Address',
          dataIndex: 'address',
          key: 'address 1',
          ellipsis: true,
        },
        {
          title: 'Long Column Long Column Long Column',
          dataIndex: 'address',
          key: 'address 2',
          ellipsis: true,
        },
        {
          title: 'Long Column Long Column',
          dataIndex: 'address',
          key: 'address 3',
          ellipsis: true,
        },
        {
          title: 'Long Column',
          dataIndex: 'address',
          key: 'address 4',
          ellipsis: true,
        },
      ],
      data: [],
    }
  },
  mounted() {
    axios.get('/api/stocks')
      .then((res) => {
        console.log(res.data);

        this.data = res.data.data || [];
      });
  },
}
</script>
