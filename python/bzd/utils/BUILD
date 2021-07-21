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
    name = "decorators",
    srcs = ["decorators.py"],
    visibility = ["//visibility:public"],
)
