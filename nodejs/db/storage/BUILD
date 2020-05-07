load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.js",
        "storage.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
    ],
)
