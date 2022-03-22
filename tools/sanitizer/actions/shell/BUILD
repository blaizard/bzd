load("@rules_python//python:python.bzl", "py_binary")

py_binary(
    name = "shell",
    srcs = [
        "shell.py",
    ],
    visibility = ["//tools/sanitizer:__pkg__"],
    deps = [
        "//tools/shell",
    ],
)
