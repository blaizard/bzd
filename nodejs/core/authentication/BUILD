load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
        "//nodejs/core:exception",
    ],
)

bzd_nodejs_library(
    name = "server",
    srcs = [
        "server.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs:bzd",
        "//nodejs/core:exception",
    ],
)
