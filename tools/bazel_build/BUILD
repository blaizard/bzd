load("@rules_cc//cc:defs.bzl", "cc_library")
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

cc_library(
    name = "empty_cc_library",
    visibility = ["//visibility:public"],
)

bzl_library(
    name = "multi_run",
    srcs = [
        "multi_run.bzl",
    ],
    visibility = ["//visibility:public"],
)
