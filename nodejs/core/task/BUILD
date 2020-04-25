load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "manager",
    srcs = [
        "manager.js",
        "task.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "manager_singleton",
    srcs = [
        "manager_singleton.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":manager",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
