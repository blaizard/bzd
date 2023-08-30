load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_binary")

bzl_library(
    name = "sh_binary_wrapper",
    srcs = [
        "sh_binary_wrapper.bzl",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "web_server",
    srcs = ["web_server.py"],
    visibility = ["//visibility:public"],
)
