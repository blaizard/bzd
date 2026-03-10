load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "handler",
    srcs = [
        "__init__.py",
    ],
    visibility = [
        "//visibility:public",
    ],
)

py_library(
    name = "async_buffered",
    srcs = [
        "async_buffered.py",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":handler",
    ],
)

py_library(
    name = "in_memory",
    srcs = [
        "in_memory.py",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":handler",
    ],
)

py_library(
    name = "stderr",
    srcs = [
        "stderr.py",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":handler",
    ],
)

py_library(
    name = "stub",
    srcs = [
        "stub.py",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":handler",
    ],
)
