load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "format",
    srcs = [
        "format.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bdl/generator/impl:visitor",
    ],
)

py_library(
    name = "composition",
    srcs = [
        "composition.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//cc/bdl/generator/impl:visitor",
    ],
)
