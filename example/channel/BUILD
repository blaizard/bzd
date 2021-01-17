load("//tools/bazel_build/rules:cc.bzl", "cc_binary", "cc_library")

cc_library(
    name = "lib",
    srcs = [
        "main.cc",
    ],
    deps = [
        "//cc/bzd",
    ],
)

cc_binary(
    name = "channel",
    visibility = ["//visibility:public"],
    deps = [
        ":lib",
    ],
)
