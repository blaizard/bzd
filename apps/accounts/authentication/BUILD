load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core/authentication/token:client",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core/authentication/session:server_proxy",
    ],
)
