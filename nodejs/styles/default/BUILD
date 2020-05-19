load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "default",
    srcs = glob([
        "**/*",
    ]),
    aliases = {
        "bzd-style": "nodejs/styles/default",
    },
    visibility = ["//visibility:public"],
)
