load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "api.json",
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
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
        "//nodejs:bzd",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:validation",
        "//nodejs/core/authentication:server",
        "//nodejs/core/authentication:user",
        "//nodejs/utils:uid",
    ],
)
