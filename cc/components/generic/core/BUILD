load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//interfaces:core",
    ],
)

cc_library(
    name = "core",
    hdrs = [
        "core.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
        "//cc/bzd/container:optional",
        "//cc/bzd/container:stack",
        "//cc/bzd/utility:constexpr_for",
    ],
)
