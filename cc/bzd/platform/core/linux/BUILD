load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_binary")

bdl_library(
    name = "bdl",
    srcs = [
        "linux.bdl",
        "main.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:core",
        "//cc/bzd/platform/generic/executor",
        "//cc/bzd/platform/out/std:std2",
    ],
)

bzd_cc_binary(
    name = "main",
    srcs = [
        "pi.hh",
    ],
    tags = ["manual"],
    deps = [
        ":linux",
        "//cc/bzd/core:delay",
        "//cc/bzd/core/async",
    ],
)
