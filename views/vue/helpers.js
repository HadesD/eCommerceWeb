import * as _ from 'lodash';
import dayjs from 'dayjs';
import { message, notification, Button, Modal, } from 'ant-design-vue';
import { defineAsyncComponent as vueDefineAsyncComponent, h } from 'vue';

import LoadingComponent from '~/components/LoadingComponent.vue';
import ErrorComponent from '~/components/ErrorComponent.vue';

export function vietnameseNormalize(str, toUpperCase = false) {
    str = str?.toLowerCase()
        .replace(/à|á|ạ|ả|ã|â|ầ|ấ|ậ|ẩ|ẫ|ă|ằ|ắ|ặ|ẳ|ẵ/g, 'a')
        .replace(/è|é|ẹ|ẻ|ẽ|ê|ề|ế|ệ|ể|ễ/g, 'e')
        .replace(/ì|í|ị|ỉ|ĩ/g, 'i')
        .replace(/ò|ó|ọ|ỏ|õ|ô|ồ|ố|ộ|ổ|ỗ|ơ|ờ|ớ|ợ|ở|ỡ/g, 'o')
        .replace(/ù|ú|ụ|ủ|ũ|ư|ừ|ứ|ự|ử|ữ/g, 'u')
        .replace(/ỳ|ý|ỵ|ỷ|ỹ/g, 'y')
        .replace(/đ/g, 'd')
        // Some system encode vietnamese combining accent as individual utf-8 characters
        .replace(/\u0300|\u0301|\u0303|\u0309|\u0323/g, '') // Huyền sắc hỏi ngã nặng
        .replace(/\u02C6|\u0306|\u031B/g, '') // Â, Ê, Ă, Ơ, Ư

        // End process
        .replace(/[^a-zA-Z0-9]/g, '-')
        .replace(/--/g, '-')
        .replace(/-$/, '')
        .replace(/^-/, '');

    return toUpperCase ? str?.toUpperCase() : str;
}

export function number_format(num) {
    return new Intl.NumberFormat().format(num || 0);
}

export function money_format(num) {
    return number_format(num) + ' ₫';
}

export function date_format(dateStr, formatStr = 'YYYY-MM-DD HH:mm:ss') {
    return dayjs(dateStr).format(formatStr);
}

export function cloneDeep(data) {
    return _.cloneDeep(data);
}

export function list_to_tree(list, parentKey = 'parent_id', key = 'id') {
    let map = {}, node, roots = [], i;

    for (i = 0; i < list.length; i += 1) {
        map[list[i][key]] = i; // initialize the map
        list[i].children = []; // initialize the children
    }

    for (i = 0; i < list.length; i += 1) {
        node = list[i];
        if (node[parentKey] !== 0) {
            // if you have dangling branches check that map[node.parentId] exists
            list[map[node[parentKey]]].children.push(node);
        } else {
            roots.push(node);
        }
    }
    return roots;
}

export function showErrorRequestApi(err) {
    if (err.response) {
        const errRes = err.response;
        if (errRes.data && errRes.data.message) {
            const key = `error${Date.now()}`;
            notification.error({
                message: 'Có lỗi xảy ra',
                description: errRes.data.message,
                btn: () => h(
                    Button,
                    {
                        type: 'primary',
                        size: 'small',
                        onClick: () => notification.close(key),
                    },
                    {
                        default: () => 'OK',
                    }
                ),
                key,
            });
        } else {
            Modal.error({
                title: (() => {
                    return `${errRes.statusText} (${errRes.status})`;
                })(),
                content: (() => {
                    switch (errRes.status) {
                        case 401:
                            return 'Bạn cần đăng nhập trước khi thực hiện thao tác này';

                        case 403:
                            return 'Bạn không có quyền thực hiện thao tác này';

                        default:
                            return 'Có lỗi xảy ra';
                    }
                })(),
            });
        }

        return;
    }

    message.error(err.message || 'Thất bại');
}

export function getFileDataBase64(file) {
    return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.readAsDataURL(file);

        reader.onload = () => resolve(reader.result);

        reader.onerror = error => reject(error);
    });
}

export function defineAsyncComponent(loader) {
    return vueDefineAsyncComponent({
        loader,
        loadingComponent: LoadingComponent,
        errorComponent: ErrorComponent,
    });
}
