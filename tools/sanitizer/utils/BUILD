load("@rules_python//python:python.bzl", "py_library")

py_library(
    name = "workspace",
    srcs = [
        "workspace.py",
    ],
    data = [
        "//tools/sanitizer:config",
    ],
    visibility = ["//tools/sanitizer:__subpackages__"],
    deps = [
        ":filter",
    ],
)

py_library(
    name = "filter",
    srcs = [
        "filter.py",
    ],
    visibility = ["//tools/sanitizer:__subpackages__"],
)
