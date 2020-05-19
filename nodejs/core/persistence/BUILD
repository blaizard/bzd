load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core:mutex",
        "//nodejs/core/task:manager_singleton",
    ],
)

bzd_nodejs_library(
    name = "memory",
    srcs = [
        "memory.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "timeseries",
    srcs = [
        "timeseries.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":disk",
        "//nodejs/core:event",
        "//nodejs/core:exception",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core:timeseries",
    ],
)
