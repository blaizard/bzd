load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_binary")

filegroup(
    name = "empty",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "merge",
    srcs = [
        "merge.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:dict",
    ],
)

bzl_library(
    name = "defs",
    srcs = [
        "defs.bzl",
    ],
    visibility = ["//visibility:public"],
)
