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

const isDashboard = process.env.BUILD_FOR === 'dashboard';

let assetRoot = isDashboard ? '/dashboard' : '';

let output = {
    chunkFilename: (assetRoot + '/assets/js/chunk/').slice(1) + (mix.inProduction() ? '[name].[chunkhash].js' : '[name].js'),
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
    const publicPath = 'public' + assetRoot;
    mix.setPublicPath(publicPath);
    mix.setResourceRoot('views');

    const assetPath = publicPath + '/assets';
    mix.js(vueDir + assetRoot + '/app-client.js', assetPath + '/js');
    mix.sass(vueDir + assetRoot + '/assets/app.scss', assetPath + '/css');

    const indexHtmlDir = 'views/index' + (isDashboard ? '-dashboard' : '') + '.html';
    if (fs.existsSync(indexHtmlDir)) {
        let htmlData = fs.readFileSync(indexHtmlDir).toString();

        mix.then((sts) => {
            if (sts.hasErrors()) {
                return;
            }
            const hostStr = !mix.inProduction() ? fs.readFileSync(publicPath + '/hot').toString().trim() : '';

            let appJs, appCss;
            let mixManifest = JSON.parse(fs.readFileSync(publicPath + '/mix-manifest.json').toString());
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
                .replace('${APP_CSS}', appCss);

            fs.writeFileSync(publicPath + '/index.html', publicIndex);
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
            port: isDashboard ? 8889 : 8888,
        },
    }
}

mix.options(options);
