load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.js",
    ],
    packages = {
        "vue": "",
    },
    deps = [
        "//nodejs/core:api",
    ],
    visibility = ["//visibility:public"],
)
