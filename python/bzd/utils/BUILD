load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "run",
    srcs = ["run.py"],
    visibility = ["//visibility:public"],
    deps = [
        "//python:bzd",
    ],
)

py_library(
    name = "worker",
    srcs = ["worker.py"],
    visibility = ["//visibility:public"],
    deps = [
        "//python:bzd",
    ],
)

py_library(
    name = "memoized_property",
    srcs = ["memoized_property.py"],
    visibility = ["//visibility:public"],
)
