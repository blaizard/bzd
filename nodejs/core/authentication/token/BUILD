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
        "//nodejs/core:mutex",
        "//nodejs/core:validation",
        "//nodejs/core/authentication:client",
        "//nodejs/core/authentication:session",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    apis = ["api.json"],
    packages = {
        "jsonwebtoken": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:validation",
        "//nodejs/core/authentication:server",
        "//nodejs/core/authentication:session",
        "//nodejs/utils:uid",
    ],
)
