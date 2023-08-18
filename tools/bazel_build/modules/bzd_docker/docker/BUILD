load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "python",
    srcs = ["python.py"],
    data = [
        "@bzd_docker//:config.json",
    ],
    visibility = ["//visibility:public"],
)
