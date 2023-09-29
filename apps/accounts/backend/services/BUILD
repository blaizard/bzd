load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "services",
    srcs = [
        "api.json",
        "services.mjs",
    ],
    visibility = [
        "//apps/accounts/backend:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)
