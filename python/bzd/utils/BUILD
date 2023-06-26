load("@rules_python//python:defs.bzl", "py_library")
load("@pip//:requirements.bzl", "requirement")

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
    name = "docker",
    srcs = ["docker.py"],
    data = [
        "//tools/docker_images:config.json",
    ],
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
