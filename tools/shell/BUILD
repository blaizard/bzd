load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "setup",
    srcs = [
        "setup.py",
    ],
    data = [
        "//tools/shell/sh",
    ],
    deps = [
        "//python/bzd:env",
    ],
)
