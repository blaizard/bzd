load("@bazel_skylib//:bzl_library.bzl", "bzl_library")
load("@rules_python//python:defs.bzl", "py_binary")
load("//config:private/common.bzl", "NB_OVERRIDE_TARGETS")

filegroup(
    name = "empty",
    visibility = ["//visibility:public"],
)

exports_files(
    ["template_py.btl"],
    visibility = ["//config:__subpackages__"],
)

label_flag(
    name = "override",
    build_setting_default = ":empty",
    visibility = ["//visibility:public"],
)

[label_flag(
    name = "override" + str(i),
    build_setting_default = ":empty",
    visibility = ["//visibility:public"],
) for i in range(0, NB_OVERRIDE_TARGETS)]

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

py_binary(
    name = "reader",
    srcs = [
        "reader.py",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "template",
    srcs = [
        "template.py",
    ],
    visibility = ["//config:__subpackages__"],
    deps = [
        "@bzd_lib//config:reader",
        "@bzd_python//bzd/template",
    ],
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
