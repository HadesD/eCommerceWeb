<template>
  <a-form-model
    ref="ruleForm"
    :model="formData"
    :rules="rules"
    :label-col="labelCol"
    :wrapper-col="wrapperCol"
    >
    <a-form-model-item @change="onNameChanged" ref="name" label="Tiêu đề" prop="name">
      <a-input
        v-model="formData.name"
        @blur="() => $refs.name.onFieldBlur"
        />
    </a-form-model-item>
    <a-form-model-item :wrapper-col="{ span: 14, offset: 4 }">
      <a-button type="primary" @click="onSubmit">
        Create
      </a-button>
      <a-button style="margin-left: 10px;" @click="resetForm">
        Reset
      </a-button>
    </a-form-model-item>
  </a-form-model>
</template>
<script>
import {vietnameseNormalize} from '../../../stringNormalize.js'

export default {
  data() {
    return {
      labelCol: { span: 4 },
      wrapperCol: { span: 14 },
      other: '',
      formData: {
        name: '',
        slug: '',
      },
      rules: {
        name: [
          { required: true, trigger: 'blur' },
          { min: 3, max: 5, trigger: 'blur' },
        ],
      },
    };
  },
  methods: {
    onNameChanged(e){
      this.formData.slug = vietnameseNormalize(e.target.value);
    },
    onSubmit() {
      this.$refs.ruleForm.validate(valid => {
        if (valid) {
          alert('submit!');
        } else {
          console.log('error submit!!');
          return false;
        }
      });
    },
    resetForm() {
      this.$refs.ruleForm.resetFields();
    },
  },
};
</script>
