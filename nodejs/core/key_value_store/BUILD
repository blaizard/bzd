load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core/persistence:disk",
        "//nodejs/core:cache",
        "//nodejs/core:event",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core:exception",
    ],
)
