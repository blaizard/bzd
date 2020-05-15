load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
        "api.json",
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
        "server.mjs",
        "api.json",
    ],
    packages = {
        "jsonwebtoken": "",
    },
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core/authentication:server",
    ],
)
