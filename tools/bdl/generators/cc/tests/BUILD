load("@rules_python//python:defs.bzl", "py_test")
load("//tools/bazel_build/rules:bdl.bzl", "bdl_library")
load("//tools/bazel_build/rules:cc.bzl", "bzd_cc_test")

bdl_library(
    name = "interface",
    srcs = [
        "manifest.bdl",
    ],
)

bzd_cc_test(
    name = "file",
    srcs = [
        "file.cc",
    ],
    deps = [
        ":interface",
        "//cc/bzd/test",
    ],
)

py_test(
    name = "template",
    srcs = [
        "template.py",
    ],
    deps = [
        "//tools/bdl:generators",
        "//tools/bdl:grammar",
    ],
)
