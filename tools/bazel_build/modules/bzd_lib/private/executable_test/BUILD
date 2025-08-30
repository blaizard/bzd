load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_binary")

package(default_visibility = ["//:__pkg__"])

bzl_library(
    name = "bzl",
    srcs = [
        "executable_test.bzl",
    ],
)

py_binary(
    name = "executable_test",
    srcs = ["executable_test.py"],
    deps = [
        "@bzd_python//bzd/utils:run",
    ],
)
