load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

filegroup(
    name = "empty",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "convert",
    srcs = [
        "convert.py",
    ],
    visibility = ["//config:__subpackages__"],
    deps = [
        "//config:reader",
        requirement("pyyaml"),
    ],
)

py_binary(
    name = "merge",
    srcs = [
        "merge.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:dict",
        requirement("pyyaml"),
    ],
)

py_binary(
    name = "reader",
    srcs = [
        "reader.py",
    ],
    visibility = ["//visibility:public"],
)

bzl_library(
    name = "defs",
    srcs = [
        "defs.bzl",
        "private/common.bzl",
        "private/config.bzl",
        "private/config_override.bzl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//lib:attrs",
    ],
)
