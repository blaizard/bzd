load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "layout",
    srcs = glob([
        "**/*.vue",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:icons",
        "//nodejs:vue",
        "//nodejs/core:localstorage",
        "//nodejs/vue/directives:tooltip",
    ],
)
