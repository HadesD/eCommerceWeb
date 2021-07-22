import * as _ from 'lodash';
import moment from 'moment';

function vietnameseNormalize(str, toUpperCase = false) {
    str = str.toLowerCase()
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
        .replace(/--/g, '-');

    return toUpperCase ? str.toUpperCase() : str;
};

function number_format(num) {
    return new Intl.NumberFormat().format(num);
};

function date_format(dateStr, formatStr = 'YYYY-MM-DD HH:mm:ss') {
    return moment(dateStr).format(formatStr);
}

function cloneDeep(data) {
    return _.cloneDeep(data);
}

function list_to_tree(list, parentKey = 'parent_id') {
    let map = {}, node, roots = [], i;

    for (i = 0; i < list.length; i += 1) {
        map[list[i].id] = i; // initialize the map
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

export {
    vietnameseNormalize, number_format,
    date_format, cloneDeep,
    list_to_tree,
};
