load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_binary")

package(default_visibility = ["//:__pkg__"])

bzl_library(
    name = "bzl",
    srcs = [
        "diff_test.bzl",
    ],
)

py_binary(
    name = "diff_test",
    srcs = ["diff_test.py"],
)
