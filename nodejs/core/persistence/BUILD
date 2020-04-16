load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "disk",
    srcs = [
        "disk.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core/task:manager_singleton",
        "//nodejs/core:event",
        "//nodejs/core:filesystem",
        "//nodejs/core:mutex",
        "//nodejs/core:log",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "memory",
    srcs = [
        "memory.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:event",
        "//nodejs/core:log",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "timeseries",
    srcs = [
        "timeseries.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":disk",
        "//nodejs/core:event",
        "//nodejs/core:timeseries",
        "//nodejs/core:filesystem",
        "//nodejs/core:log",
        "//nodejs/core:exception",
    ],
)
