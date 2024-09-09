load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_binary")

exports_files(["executable_test.py"])

py_binary(
    name = "json_merge",
    srcs = [
        "json_merge.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:dict",
    ],
)

bzl_library(
    name = "multiplatform_repository",
    srcs = [
        "multiplatform_repository.bzl",
    ],
    visibility = ["//visibility:public"],
)

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
