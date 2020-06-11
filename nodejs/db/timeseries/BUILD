load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.mjs",
        "timeseries.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:cache",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core/persistence:timeseries",
    ],
)
