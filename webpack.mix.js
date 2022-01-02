const mix = require('laravel-mix');
const path = require('path');
const fs = require('fs');

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
    chunkFilename: 'assets/js/chunk/' + (mix.inProduction() ? '[name].[chunkhash].js' : '[name].js'),
};

let options = {
    // manifest: true,
};

let vueDir = './views/vue';

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
    mix.js(vueDir + '/app-client.js', 'public/assets/js');
    mix.sass(vueDir + '/assets/app.scss', 'public/assets/css');

    const indexHtmlDir = 'views/index.html';
    if (fs.existsSync(indexHtmlDir)) {
        let htmlData = fs.readFileSync(indexHtmlDir).toString();
        const hostStr = !mix.inProduction() ? fs.readFileSync('hot').toString().trim() : 0;

        mix.then((sts) => {
            if (sts.hasErrors()) {
                return;
            }

            let appJs, appCss;
            let mixManifest = JSON.parse(fs.readFileSync('mix-manifest.json').toString());
            for (const i in mixManifest) {
                if (i.indexOf('.js') !== -1) {
                    appJs = mixManifest[i];
                } else if (i.indexOf('.css') !== -1) {
                    appCss = mixManifest[i];
                }
            }

            let publicIndex = htmlData
                .replace('${HOST}', hostStr)
                .replace('${HOST}', hostStr)
                .replace('${APP_JS}', appJs)
                .replace('${APP_CSS}', appCss)
                .trim();
            console.log(publicIndex);
        });
    }
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
