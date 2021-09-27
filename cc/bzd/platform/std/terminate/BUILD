load("//tools/bazel_build/rules:cc.bzl", "cc_library")

cc_library(
    name = "terminate",
    srcs = [
        "terminate.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:panic",
    ],
    alwayslink = True,
)
