load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "factory",
    srcs = [
        "factory.py",
    ],
    data = [
        ".travis.btl",
    ],
    visibility = [
        "//tools/ci:__pkg__",
    ],
    deps = [
        "//tools/ci:factory",
    ],
)
