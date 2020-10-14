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
        "vue": "",
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
        "babel-loader": "",
        "babel-plugin-syntax-dynamic-import": "",
        # Webpack
        "webpack": "4.44.2",
        "webpack-cli": "3.3.12",
        "webpack-assets-manifest": "",
        "webpack-bundle-analyzer": "",
        # Vue
        "vue-loader": "^15",
        "vue-template-compiler": "",
        # CSS/SASS
        "mini-css-extract-plugin": "0.8.0",  # Seems that 0.9.0 has some issue: https://github.com/webpack-contrib/mini-css-extract-plugin/issues/493
        "css-loader": "3.6.0",  # From 4.0.0 the keyword :export seems broken
        "node-sass": "",
        "sass-loader": "",
        "sass": "",
        "file-loader": "",
        "url-loader": "",
    },
    visibility = ["//visibility:public"],
)
