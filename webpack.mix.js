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

// mix.webpackConfig({
//     output: {
//         chunkFilename: mix.inProduction() ? 'js/chunks/[name].[chunkhash].js' : 'js/chunks/[name].js',
//     },
// });

mix.js('resources/vue/dashboard/app.js', 'public/js/dashboard').vue();
mix.sass('resources/vue/assets/dashboard/app.scss', 'public/css/dashboard');

mix.browserSync({
    proxy: 'rinphone.local',
});

if (mix.inProduction()) {
    mix.version();
} else {
    mix.sourceMaps();
}
