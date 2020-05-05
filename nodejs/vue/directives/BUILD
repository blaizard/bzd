load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "touch",
    srcs = [
        "touch.js",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "tooltip",
    srcs = [
        "tooltip.js",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "resize",
    srcs = [
        "resize.js",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "mask",
    srcs = [
        "mask.js",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "loading",
    srcs = [
        "loading.js",
    ],
    deps = [
        ":mask",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)
