load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "clock",
    srcs = [
        "clock.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:clock",
    ],
)

bdl_library(
    name = "core",
    srcs = [
        "core.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform/interfaces/cc:core",
    ],
)

bdl_library(
    name = "executor",
    srcs = [
        "executor.bdl",
    ],
    visibility = ["//visibility:public"],
)

bdl_library(
    name = "gateway",
    srcs = [
        "gateway.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform/interfaces/cc:gateway",
    ],
)

bdl_library(
    name = "recorder",
    srcs = [
        "recorder.bdl",
    ],
    visibility = ["//visibility:public"],
)

bdl_library(
    name = "stream",
    srcs = [
        "stream.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/core:channel",
    ],
)

bdl_library(
    name = "tcp",
    srcs = [
        "tcp.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform/interfaces/cc:tcp",
    ],
)
