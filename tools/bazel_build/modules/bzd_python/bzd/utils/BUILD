load("@bzd_python_pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "run",
    srcs = ["run.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "worker",
    srcs = ["worker.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "file",
    srcs = ["file.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "decorators",
    srcs = ["decorators.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "dict",
    srcs = ["dict.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "logging",
    srcs = ["logging.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "result",
    srcs = ["result.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "uart",
    srcs = ["uart.py"],
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyserial"),
    ],
)
