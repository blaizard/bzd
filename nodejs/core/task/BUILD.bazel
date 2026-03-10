load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "manager",
    srcs = [
        "manager.mjs",
        "task.mjs",
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
        "manager_singleton.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":manager",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
