load("@rules_python//python:python.bzl", "py_library")

py_library(
    name = "workspace",
    srcs = [
        "workspace.py",
    ],
    data = [
        "//tools/sanitizer:config",
    ],
    deps = [
        ":filter"
    ],
    visibility = ["//tools/sanitizer:__subpackages__"],
)

py_library(
    name = "filter",
    srcs = [
        "filter.py",
    ],
    visibility = ["//tools/sanitizer:__subpackages__"],
)
