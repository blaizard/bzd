load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/http:client",
    ],
)

bzd_nodejs_library(
    name = "server_context",
    srcs = [
        "server_context.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:stream",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":server_context",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:router",
    ],
)
