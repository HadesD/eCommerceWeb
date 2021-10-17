<template>
    <a-upload
        accept="image/*"
        list-type="picture-card"
        v-model:fileList="fileList"
        @preview="preview"
        @change="change"
        :beforeUpload="beforeUpload"
    >
        <div v-if="fileList.length < 8">
            <PlusOutlined />
            <div class="ant-upload-text">Upload</div>
        </div>
    </a-upload>
    <a-modal
        :visible="previewImgVisible"
        :footer="null"
        @cancel="cancelPreview"
    >
        <img alt="example" style="width: 100%" :src="previewImgUrl" />
    </a-modal>
</template>
<script>
import { ref } from '@vue/reactivity';

import { getFileDataBase64 } from "../../helpers";

import {
    PlusOutlined,
} from "@ant-design/icons-vue";

export default {
    components: {
        PlusOutlined,
    },

    setup() {
        const previewImgVisible = ref(false);
        const fileList = ref([]);
        const previewImgUrl = ref();

        const beforeUpload = (file) => {
            fileList.value = [...fileList.value, file];

            return false;
        };

        const preview = async (file) => {
            if (!file.url && !file.preview) {
                file.preview = await getFileDataBase64(file.originFileObj);
            }

            previewImgUrl.value = file.url || file.preview;
            previewImgVisible.value = true;
        };

        const change = () => {
            const fileItem = fileList.value[fileList.value.length - 1];

            const data = new FormData();
            data.append( 'image', fileItem.originFileObj);

            const xhttp = new XMLHttpRequest();
            xhttp.open('POST', 'https://api.imgur.com/3/image');
            xhttp.setRequestHeader('Authorization', 'Client-ID ed03938f3ff9c55'); //Get yout Client ID here: http://api.imgur.com/
            xhttp.onreadystatechange = () => {
                if ((xhttp.status === 200) && (xhttp.readyState === 4)) {
                    const res = JSON.parse(xhttp.responseText);
                    console.log(res);
                    fileItem.status = 'done';
                    fileItem.url = res.data.link;
                }
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
            previewImgUrl,

            beforeUpload,
            preview,
            change,
            cancelPreview,
        };
    },
}
</script>
