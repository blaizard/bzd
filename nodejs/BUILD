load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "bzd",
    aliases = {
        "bzd": "nodejs",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "vue",
    packages = {
        "vue": "2.7.13",
    },
    visibility = ["//visibility:public"],
    deps = [
        ":bzd",
    ],
)

bzd_nodejs_library(
    name = "icons",
    srcs = [
        "icons.scss",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bzd",
    ],
)

bzd_nodejs_library(
    name = "webpack",
    srcs = [
        "template.cjs",
        "webpack.cjs",
    ],
    packages = {
        # Babel
        "@babel/core": "",
        "@babel/plugin-transform-runtime": "",
        "babel-loader": "^8.3.0",
        "babel-plugin-syntax-dynamic-import": "",
        "css-loader": "3.6.0",  # From 4.0.0 the keyword :export seems broken
        "file-loader": "6.2.0",
        # CSS/SASS
        "mini-css-extract-plugin": "0.8.0",  # Seems that 0.9.0 has some issue: https://github.com/webpack-contrib/mini-css-extract-plugin/issues/493
        "node-sass": "7.0.3",
        "sass": "1.35.1",
        "sass-loader": "10.1.1",  # Breaking changes from 11 onward, only support webpack v5+
        "url-loader": "4.1.1",
        # Vue
        "vue-loader": "^15",
        "vue-template-compiler": "2.6.14",
        # Webpack
        "webpack": "4.44.2",
        "webpack-assets-manifest": "3.1.1",
        "webpack-bundle-analyzer": "",
        "webpack-cli": "3.3.12",
    },
    visibility = ["//visibility:public"],
)
