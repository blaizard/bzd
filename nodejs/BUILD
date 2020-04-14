load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "vue",
    srcs = glob([
        "vue/**/*",
    ]),
    alias = "bzd",
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "icons",
    srcs = glob([
        "assets/svg/*.svg",
    ]) + ["icons.scss"],
    alias = "bzd",
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "styles",
    srcs = glob([
        "assets/style/*",
    ]),
    alias = "bzd",
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "webpack",
    srcs = [
        "webpack.js",
    ],
    alias = "bzd",
    packages = {
        # Babel
        "@babel/core": "",
        "@babel/plugin-transform-runtime": "",
        "babel-loader": "",
        "babel-plugin-syntax-dynamic-import": "",
        # Webpack
        "webpack": "",
        "webpack-cli": "",
        "webpack-assets-manifest": "",
        "webpack-bundle-analyzer": "",
        # Vue
        "vue-loader": "",
        "vue-template-compiler": "",
        "vue-style-loader": "",
        # CSS/SASS
        "mini-css-extract-plugin": "0.8.0",  # Seems that 0.9.0 has some issue: https://github.com/webpack-contrib/mini-css-extract-plugin/issues/493
        "css-loader": "",
        "node-sass": "",
        "sass-loader": "",
        "file-loader": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/require:core",
        "//nodejs/require:template",
    ],
)
