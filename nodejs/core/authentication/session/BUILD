load("@bzd_rules_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:validation",
        "//nodejs/core/authentication:server",
        "//nodejs/core/authentication:user",
        "//nodejs/utils:uid",
    ],
)
