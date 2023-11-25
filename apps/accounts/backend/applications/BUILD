load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "applications",
    srcs = [
        "application.mjs",
        "applications.mjs",
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
        "applications.vue",
    ],
    apis = ["api.json"],
    visibility = [
        "//apps/accounts:__subpackages__",
    ],
    deps = [
        "//nodejs/db:utils",
        "//nodejs/vue/components/form",
        "//nodejs/vue/components/layout:component",
        "//nodejs/vue/components/modal",
        "//nodejs/vue/directives:tooltip",
    ],
)
