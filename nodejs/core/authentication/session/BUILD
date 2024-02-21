load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
    ],
    apis = ["api.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/authentication:client",
    ],
)

bzd_nodejs_library(
    name = "server_proxy",
    srcs = [
        "server_proxy.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":server",
        "//nodejs/core/rest:client",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
        ":api.json",
    ],
    apis = ["api.json"],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/authentication:server",
        "//nodejs/core/authentication:session",
        "//nodejs/db/key_value_store:memory",
    ],
)
