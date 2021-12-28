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

let output = {
    chunkFilename: 'public/assets/js/chunk/' + (mix.inProduction() ? '[name].[chunkhash].js' : '[name].js'),
};

let options = {
    manifest: false,
};

mix.webpackConfig({
    resolve: {
        alias: {
            '~': path.join(__dirname, './views/vue')
        }
    },
});

if (process.env.RENDER_TARGET === 'server') {
    output = {
        ...output,
        globalObject: 'this',
    };

    mix.js('views/vue/app-server.js', 'public/assets/js')
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
    mix.js('views/vue/app-client.js', 'public/assets/js');
    mix.sass('views/vue/assets/app.scss', 'public/assets/css');

    mix.js('views/vue/dashboard/app.js', 'dashboard/public/assets/js');
    mix.sass('views/vue/dashboard/assets/app.scss', 'dashboard/public/assets/css');
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
