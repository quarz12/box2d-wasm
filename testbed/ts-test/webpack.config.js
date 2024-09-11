const path = require('path');

module.exports = {
    entry: './src/demo2.ts',
    devtool: 'inline-source-map',
    module: {
        rules: [
            {
                test: /\.tsx?$/,
                use: 'ts-loader',
                exclude: /node_modules/,
            },
        ],
    },
    resolve: {
        modules: ['.', 'node_modules'],
        extensions: ['.tsx', '.ts', '.js'],
        fallback: {
            "path": require.resolve("path-browserify"),
            fs:false,
        },
    },
    output: {
        filename: 'bundle.js',
        path: path.resolve(__dirname, 'build'),
    },

};