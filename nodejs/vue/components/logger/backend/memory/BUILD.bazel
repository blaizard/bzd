load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "memory",
    srcs = [
        "memory.mjs",
    ],
    apis = ["//nodejs/vue/components/logger:api.json"],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
