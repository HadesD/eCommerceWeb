<template>
    <a-alert message="Chỉ tải ảnh cần thiết, không được chọn ảnh thừa linh tinh, tránh gây tốn dung lượng máy chủ!" type="warning" show-icon style="margin-bottom: 10px;" />
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

import { getFileDataBase64 } from '~/helpers';

import {
    PlusOutlined,
} from '@ant-design/icons-vue';
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
            // TODO: Fix 4 time being called
            fileList.value = value || [];
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
            const file = info.file;

            // Is update event
            if (file.originFileObj) {
                return;
            }

            // Is uploaded
            if (file.url) {
                return;
            }

            const fileItem = fileList.value.find((v) => {
                return v.originFileObj === file;
            });

            const data = new FormData();
            data.append('image', file);

            const imgurClientIds = [
                'ed03938f3ff9c55',
                'fada7b81b37ee0a',
                '44d107a9b53bbf5',
                'be1c5cf3222ac27',
            ];

            const xhttp = new XMLHttpRequest();
            xhttp.open('POST', 'https://api.imgur.com/3/image');
            xhttp.setRequestHeader('Authorization', 'Client-ID ' + imgurClientIds[_.random(imgurClientIds.length - 1)]); //Get yout Client ID here: http://api.imgur.com/
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
