const mix = require('laravel-mix');
const path = require('path');

require('mix-env-file');

mix.env('./.env');

/*
 |--------------------------------------------------------------------------
 | Mix Asset Management
 |--------------------------------------------------------------------------
 |
 | Mix provides a clean, fluent API for defining some Webpack build steps
 | for your Laravel applications. By default, we are compiling the CSS
 | file for the application as well as bundling up all the JS files.
 |
 */

const isDashboard = process.env.BUILD_FOR === 'dashboard';

let assetRoot = isDashboard ? '/dashboard' : '';

let output = {
    chunkFilename: assetRoot + '/assets/js/chunk/' + (mix.inProduction() ? '[name].[chunkhash].js' : '[name].js'),
};

let options = {
    // manifest: false,
};

let vueDir = './resources/vue';

mix.webpackConfig({
    resolve: {
        alias: {
            '~': path.join(__dirname, vueDir)
        }
    },
});

if (process.env.RENDER_TARGET === 'server') {
    output = {
        ...output,
        globalObject: 'this',
    };

    mix.js(vueDir + '/app-server.js', 'public/assets/js')
        .webpackConfig({
            // target: 'node',
            resolve: {
                mainFields: [
                    "main", "module",
                ],
                fallback: {
                    stream: false,
                },
            },
        });
} else {
    mix.js(vueDir + '/dashboard/app.js', 'public/assets/js');
    mix.sass(vueDir + '/dashboard/assets/app.scss', 'public/assets/css');
}

mix.vue();

mix.webpackConfig({
    output,
});

if (mix.inProduction()) {
    mix.version();
} else {
    mix.sourceMaps();

    // Fix error can not create tcp socket
    options = {
        ...options,
        hmrOptions: {
            host: '127.0.0.1',
            port: 8888,
        },
    }
}

mix.options(options);
