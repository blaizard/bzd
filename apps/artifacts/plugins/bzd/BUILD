load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
        "nodes.mjs",
        "storage.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__pkg__",
    ],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/db:utils",
        "//nodejs/db/storage",
    ],
)

bzd_nodejs_library(
    name = "frontend",
    srcs = [
        "frontend.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__pkg__",
    ],
    deps = [
        ":view",
        "//apps/artifacts/plugins:default",
    ],
)

bzd_nodejs_library(
    name = "view",
    srcs = [
        "view_category.vue",
        "view_node.vue",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//apps/artifacts/plugins:default",
    ],
)
