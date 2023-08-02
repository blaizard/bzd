load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
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
