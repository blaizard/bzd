load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "bdl",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        "//cc/bzd/platform/interfaces:clock",
    ],
)

cc_library(
    name = "system_clock",
    srcs = [
        "system_clock.cc",
    ],
    hdrs = [
        "system_clock.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":bdl",
    ],
)
