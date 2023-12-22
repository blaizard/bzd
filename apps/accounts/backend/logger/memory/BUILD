load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "memory",
    srcs = [
        "memory.mjs",
    ],
    apis = ["api.json"],
    visibility = [
        "//apps/accounts/backend:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "memory.vue",
    ],
    apis = ["api.json"],
    visibility = [
        "//apps/accounts:__subpackages__",
    ],
)
