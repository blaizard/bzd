load("@rules_python//python:defs.bzl", "py_binary")
load("@pip//:requirements.bzl", "requirement")
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

filegroup(
    name = "metadata_json",
    srcs = ["metadata.json"],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "profiler",
    srcs = ["profiler.py"],
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyelftools"),
    ],
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
