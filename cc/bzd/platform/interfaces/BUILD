load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "core",
    srcs = [
        "core.bdl",
    ],
    visibility = ["//visibility:public"],
)

bdl_library(
    name = "executor",
    srcs = [
        "executor.bdl",
    ],
    visibility = ["//visibility:public"],
)

bdl_library(
    name = "stream",
    srcs = [
        "stream.bdl",
    ],
    deps = [
        "//cc/bzd/core:channel",
    ],
    visibility = ["//visibility:public"],
)
