load("@bzd_python_pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_binary(
    name = "binary_builder",
    srcs = ["binary_builder.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "run",
    srcs = ["run.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "semver",
    srcs = ["semver.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "ssh",
    srcs = [
        "ssh.py",
        "//bzd:bin.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":run",
        "//bzd/http:client",
        requirement("paramiko"),
    ],
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
    name = "match",
    srcs = ["match.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "timeout",
    srcs = ["timeout.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "result",
    srcs = ["result.py"],
    visibility = ["//visibility:public"],
)

py_library(
    name = "scheduler",
    srcs = ["scheduler.py"],
    visibility = ["//visibility:public"],
    deps = [
        "//bzd/logging",
    ],
)

py_library(
    name = "uart",
    srcs = ["uart.py"],
    visibility = ["//visibility:public"],
    deps = [
        requirement("pyserial"),
    ],
)
