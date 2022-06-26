load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    deps = [
        ":interface_cc",
    ],
)

cc_library(
    name = "interface_cc",
    hdrs = [
        "interface_cc.hh",
    ],
)

bzd_cc_test(
    name = "cc",
    srcs = [
        "implementation.cc",
    ],
    deps = [
        ":interface",
        "//cc/bzd/test",
    ],
)
