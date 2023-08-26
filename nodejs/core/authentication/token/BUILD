load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "api.json",
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/authentication:client",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "api.json",
        "server.mjs",
    ],
    packages = {
        "jsonwebtoken": "",
    },
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
