load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "interface",
    srcs = [
        "interface.bdl",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "std2",
    hdrs = [
        "std.hh",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":interface",
        "//cc/bzd/core:channel",
    ],
)

cc_library(
    name = "std",
    srcs = [
        "std.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bzd/platform:headers",
    ],
    alwayslink = True,
)
