load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_binary")

exports_files([
    "defs.bzl",
])

bzl_library(
    name = "defs",
    srcs = [
        "defs.bzl",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "package",
    srcs = ["package.py"],
    visibility = ["//visibility:public"],
)
