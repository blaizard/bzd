load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "backend",
    srcs = [
        "backend.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__pkg__",
    ],
    deps = [
        "//apps/artifacts/backend:plugin",
        "//apps/artifacts/plugins/extension/release",
        "//nodejs/db/storage:webdav",
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
        "//apps/artifacts/plugins:default",
    ],
)
