load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "coverage",
    srcs = glob([
        "*",
    ]),
    visibility = [
        "//apps/dashboard/plugins:__pkg__",
    ],
)
