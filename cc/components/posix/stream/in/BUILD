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
        "//interfaces:stream",
    ],
)

cc_library(
    name = "in",
    hdrs = [
        "in.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
    ],
)
