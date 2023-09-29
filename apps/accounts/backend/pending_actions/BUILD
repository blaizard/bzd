load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "pending_actions",
    srcs = [
        "api.json",
        "pending_actions.mjs",
    ],
    visibility = [
        "//apps/accounts/backend:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/db:utils",
    ],
)
