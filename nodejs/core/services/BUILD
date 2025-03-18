load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "services.vue",
    ],
    apis = ["api.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/utils:to_string",
    ],
)

bzd_nodejs_library(
    name = "services",
    srcs = [
        "process.mjs",
        "provider.mjs",
        "services.mjs",
    ],
    apis = ["api.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
