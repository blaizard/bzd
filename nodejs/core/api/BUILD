load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "base.mjs",
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:format",
        "//nodejs/core:validation",
        "//nodejs/core/http:client",
        "//nodejs/core/http:endpoint",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "base.mjs",
        "server.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:validation",
    ],
)
