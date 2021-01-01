load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "touch",
    srcs = [
        "touch.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "tooltip",
    srcs = [
        "tooltip.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "resize",
    srcs = [
        "resize.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "mask",
    srcs = [
        "mask.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "loading",
    srcs = [
        "loading.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        ":mask",
    ],
)
