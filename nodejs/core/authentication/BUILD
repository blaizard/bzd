load("@bzd_toolchain_nodejs//nodejs:defs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "user",
    srcs = [
        "user.mjs",
    ],
    visibility = ["//visibility:public"],
)

bzd_nodejs_library(
    name = "client",
    srcs = [
        "client.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
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
        "//nodejs/core:exception",
    ],
)
