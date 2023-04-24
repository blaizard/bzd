load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//cc/bzd/platform/interfaces:timer",
    ],
)

cc_library(
    name = "steady_clock",
    srcs = [
        "steady_clock.cc",
    ],
    hdrs = [
        "steady_clock.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
    ],
)
