load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "vue",
    packages = {
        "vue": "2.7.13",
    },
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "icons",
    srcs = [
        "icons.scss",
    ],
    visibility = ["//visibility:public"],
)
