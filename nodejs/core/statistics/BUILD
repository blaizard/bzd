load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "statistics.vue",
    ],
    apis = ["api.json"],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "statistics",
    srcs = [
        "provider.mjs",
        "statistics.mjs",
    ],
    apis = ["api.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
