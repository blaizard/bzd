load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_binary")

bzl_library(
    name = "bzl",
    srcs = [
        "defs.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//private/diff_test:bzl",
        "//private/executable_test:bzl",
    ],
)

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
    name = "repository_multiplatform_maker",
    srcs = [
        "repository_multiplatform_maker.bzl",
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
