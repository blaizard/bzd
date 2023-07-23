load("@rules_python//python:defs.bzl", "py_binary")
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

py_binary(
    name = "executor_host",
    srcs = [
        "executor_host.py",
    ],
    visibility = ["//visibility:public"],
)

bzl_library(
    name = "defs",
    srcs = [
        "defs.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bazel_tools//tools/cpp:toolchain_utils.bzl",
    ],
)
