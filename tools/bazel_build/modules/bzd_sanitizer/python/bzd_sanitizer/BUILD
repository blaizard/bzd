load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "context",
    srcs = [
        "context.py",
        "filter.py",
    ],
    imports = [".."],
    visibility = [
        "//visibility:public",
    ],
)

py_library(
    name = "worker",
    srcs = [
        "worker.py",
    ],
    visibility = [
        "//:__subpackages__",
    ],
    deps = [
        ":context",
        "@bzd_python//bzd/utils:worker",
    ],
)
