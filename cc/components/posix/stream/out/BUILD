load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/components/posix/proactor",
    ],
)

cc_library(
    name = "out",
    hdrs = [
        "out.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/bzd/core:channel",
    ],
)
