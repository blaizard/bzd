load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "users",
    srcs = [
        "user.mjs",
        "users.mjs",
    ],
    apis = ["api.json"],
    visibility = [
        "//apps/accounts/backend:__subpackages__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/db:utils",
    ],
)

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "users.vue",
    ],
    apis = ["api.json"],
    visibility = [
        "//apps/accounts:__subpackages__",
    ],
    deps = [
        "//nodejs/db:utils",
        "//nodejs/utils:object",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/components/modal",
        "//nodejs/vue/directives:tooltip",
    ],
)
