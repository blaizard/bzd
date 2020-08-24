load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "base.mjs",
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
        "//nodejs/core:exception",
        "//nodejs/core:validation",
        "//nodejs/core/http:client",
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
        "//nodejs:bzd",
        "//nodejs/core:exception",
        "//nodejs/core:log",
        "//nodejs/core:validation",
    ],
)
