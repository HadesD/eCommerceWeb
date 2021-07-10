const mix = require('laravel-mix');

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
    chunkFilename: mix.inProduction() ? 'js/chunks/[name].[chunkhash].js' : 'js/chunks/[name].js',
};

if (process.env.RENDER_TARGET === 'server') {
    output = {
        ...output,
        globalObject: 'this',
    };

    mix.js('resources/vue/app-server.js', 'public/js')
        .webpackConfig({
            target: 'node',
        });
} else {
    mix.js('resources/vue/app-client.js', 'public/js');
    mix.sass('resources/vue/assets/app.scss', 'public/css');

    mix.js('resources/vue/dashboard/app.js', 'public/js/dashboard');
    mix.sass('resources/vue/assets/dashboard/app.scss', 'public/css/dashboard');

    const HOST = 'rinphone.local'
    mix.browserSync({
        host: HOST,
        proxy: HOST,
        open: 'external',
    });
}

mix.vue();

mix.webpackConfig({
    output,
});

if (mix.inProduction()) {
    mix.version();
} else {
    mix.sourceMaps();
}
