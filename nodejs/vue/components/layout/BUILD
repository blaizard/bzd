load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "layout",
    srcs = glob([
        "**/*",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:icons",
        "//nodejs:vue",
    ],
)
