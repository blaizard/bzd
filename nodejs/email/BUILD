load("//tools/bazel_build/rules:nodejs.bzl", "bzd_nodejs_library")

bzd_nodejs_library(
    name = "sendgrid",
    srcs = [
        "email.mjs",
        "sendgrid.mjs",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//nodejs/core:exception",
        "//nodejs/core:fetch",
        "//nodejs/core:log",
    ],
)
