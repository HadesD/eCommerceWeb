<template>
    <a-upload
        accept="image/*"
        list-type="picture-card"
        v-model:fileList="fileList"
        :beforeUpload="beforeUpload"
        @preview="preview"
        @change="change"
    >
        <div v-if="fileList.length < 8">
            <PlusOutlined />
            <div class="ant-upload-text">Upload</div>
        </div>
        <div v-for="(file, index) in fileList">
            <slot name="imageRender" :file="file"></slot>
        </div>
    </a-upload>
    <a-modal
        :visible="previewImgVisible"
        :footer="null"
        @cancel="cancelPreview"
    >
        <img :alt="previewImg?.name" style="width: 100%" :src="previewImg?.url || previewImg?.preview" />
    </a-modal>
</template>
<script>
import { ref } from '@vue/reactivity';

import { getFileDataBase64 } from "../../helpers";

import {
    PlusOutlined,
} from "@ant-design/icons-vue";
import { watch } from '@vue/runtime-core';

export default {
    props: {
        defaultImages: Array,
        change: Function,
    },
    emits: [],

    components: {
        PlusOutlined,
    },

    setup(props) {
        const previewImgVisible = ref(false);
        const fileList = ref([]);
        const previewImg = ref({});

        watch(() => props.defaultImages, (value, prevValue) => {
            console.log('defaultImages', value);
            fileList.value = value;
        });

        watch(() => fileList.value, (value) => {
            props.change(value);
        });

        const beforeUpload = (file) => {
            fileList.value = [...fileList.value, file];

            return false;
        };

        const preview = async (file) => {
            if (!file.url && !file.preview) {
                file.preview = await getFileDataBase64(file.originFileObj);
            }

            previewImg.value = file;
            previewImgVisible.value = true;
        };

        const change = (info) => {
            const fileItem = info.file;

            // Is update event
            if (fileItem.originFileObj) {
                return;
            }

            // Is uploaded
            if (fileItem.url) {
                return;
            }

            const data = new FormData();
            data.append('image', fileItem);

            const xhttp = new XMLHttpRequest();
            xhttp.open('POST', 'https://api.imgur.com/3/image');
            xhttp.setRequestHeader('Authorization', 'Client-ID ed03938f3ff9c55'); //Get yout Client ID here: http://api.imgur.com/
            xhttp.responseType = 'json';
            xhttp.onload = () => {
                if (xhttp.status !== 200) {
                    fileItem.status = 'error';
                    return;
                }
                const res = xhttp.response;
                fileItem.status = 'done';
                fileItem.url = res.data.link;

                props.change(fileList.value);
            };
            xhttp.onloadstart = () => {
                fileItem.status = 'uploading';
            };
            xhttp.upload.onprogress = (pe) => {
                if(pe.lengthComputable) {
                    fileItem.percent = (pe.loaded / pe.total) * 100;
                }
            };
            xhttp.onloadend = (pe) => {
                fileItem.percent = 100;
            };
            xhttp.onerror = (ev) => {
                fileItem.status = 'error';
            };
            xhttp.onabort = (ev) => {
                fileItem.status = 'error';
            };
            xhttp.send(data);
        };

        const cancelPreview = () => {
            previewImgVisible.value = false;
            // previewImgUrl.value = null;
        };

        return {
            previewImgVisible,
            fileList,
            previewImg,

            beforeUpload,
            preview,
            change,
            cancelPreview,
        };
    },
}
</script>
