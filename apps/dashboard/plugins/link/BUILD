load("//tools/bazel.build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "link",
    srcs = glob([
        "*",
    ]),
    visibility = [
        "//apps/dashboard/plugins:__pkg__",
    ],
)
