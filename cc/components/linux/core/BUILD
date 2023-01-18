load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//cc/bzd/platform/interfaces:core",
    ],
)

cc_library(
    name = "core",
    hdrs = [
        "core.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:stack",
        "//cc/bzd/core:error",
        "//cc/bzd/math:ceil",
        "//cc/bzd/utility:align_up",
        "//cc/components/posix:error",
        "//cc/libs/pthread",
        "//cc/bzd/core:print"
    ],
)
