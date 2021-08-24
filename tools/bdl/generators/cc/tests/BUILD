load("@rules_cc//cc:defs.bzl", "cc_test")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "interface",
    srcs = [
        "manifest.bdl",
    ],
)

cc_test(
    name = "file",
    srcs = [
        "file.cc",
    ],
    deps = [
        ":interface",
        "//tools/cc_test",
    ],
)
