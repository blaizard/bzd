load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "nodes",
    srcs = [
        "nodes.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        ":source",
        "//apps/artifacts/plugins/nodes",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "source",
    srcs = [
        "source.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        "//apps/artifacts/plugins/nodes",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/services",
    ],
)
