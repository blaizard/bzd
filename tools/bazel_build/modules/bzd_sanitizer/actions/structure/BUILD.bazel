load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "structure",
    srcs = [
        "structure.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd_sanitizer:worker",
    ],
)
