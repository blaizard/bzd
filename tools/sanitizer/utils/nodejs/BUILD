load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "filter",
    srcs = [
        "filter.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "workspace",
    srcs = [
        "workspace.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":filter",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
    ],
)
