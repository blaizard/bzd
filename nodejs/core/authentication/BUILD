load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "session",
    srcs = [
        "session.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":scopes",
    ],
)

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "scopes",
    srcs = [
        "scopes.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/utils:result",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
    ],
)
