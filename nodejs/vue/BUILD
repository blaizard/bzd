load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "api",
    srcs = [
        "api.mjs",
    ],
    packages = {
        "vue": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:api",
    ],
)
