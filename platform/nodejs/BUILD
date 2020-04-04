load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "webpack",
    srcs = [
        "require/exception.js",
        "require/log.js",
        "require/template.js",
        "webpack.js",
    ],
    packages = {
        # Babel
        "@babel/core": "latest",
        "@babel/plugin-transform-runtime": "latest",
        "babel-loader": "latest",
        "babel-plugin-syntax-dynamic-import": "latest",
        # Webpack
        "webpack": "latest",
        "webpack-cli": "latest",
        "webpack-assets-manifest": "latest",
        "webpack-bundle-analyzer": "latest",
        # Vue
        "vue-loader": "latest",
        "vue-template-compiler": "latest",
        "vue-style-loader": "latest",
        # CSS/SASS
        "mini-css-extract-plugin": "latest",
        "css-loader": "latest",
        "sass-loader": "latest",
        "file-loader": "latest",
    },
    visibility = ["//visibility:public"],
)
