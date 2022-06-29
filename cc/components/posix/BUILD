load("@rules_cc//cc:defs.bzl", "cc_library")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")

bdl_library(
    name = "posix",
    srcs = [
        "posix.bdl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":file_descriptor"
    ]
)

cc_library(
    name = "error",
    hdrs = [
        "error.hh",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/bzd/container:result",
        "//cc/bzd/core:error",
    ],
)

cc_library(
    name = "file_descriptor",
    hdrs = [
        "file_descriptor.hh",
    ],
    srcs = [
        "file_descriptor.cc",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//cc/bzd/utility:ownership",
    ],
)
