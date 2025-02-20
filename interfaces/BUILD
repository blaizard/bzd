load("@bzd_bdl//:defs.bzl", "bdl_library")

bdl_library(
    name = "clock",
    srcs = [
        "clock.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform/interfaces:clock",
    ],
)

bdl_library(
    name = "core",
    srcs = [
        "core.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform/interfaces:core",
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
        "//cc/bzd/platform/interfaces:gateway",
    ],
)

bdl_library(
    name = "io",
    srcs = [
        "io.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform/interfaces:io",
    ],
)

bdl_library(
    name = "network",
    srcs = [
        "network.bdl",
    ],
    visibility = ["//visibility:public"],
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
        ":network",
        "//cc/bzd/platform/interfaces:tcp",
    ],
)

bdl_library(
    name = "timer",
    srcs = [
        "timer.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform/interfaces:timer",
    ],
)
