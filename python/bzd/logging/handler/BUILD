load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "handler",
    srcs = [
        "__init__.py",
    ],
    visibility = [
        "//bzd/logging:__subpackages__",
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
