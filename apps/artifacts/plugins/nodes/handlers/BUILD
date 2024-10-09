load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "handlers",
    srcs = [
        "handlers.mjs",
    ],
    visibility = [
        "//apps/artifacts/plugins:__subpackages__",
    ],
    deps = [
        ":history",
        ":to_string",
        "//apps/artifacts/plugins/nodes:key_mapping",
        "//nodejs/core:exception",
        "//nodejs/core:log",
    ],
)

bzd_nodejs_library(
    name = "history",
    srcs = [
        "history.mjs",
    ],
)

bzd_nodejs_library(
    name = "to_string",
    srcs = [
        "to_string.mjs",
    ],
    deps = [
        "//nodejs/core:format",
    ],
)
