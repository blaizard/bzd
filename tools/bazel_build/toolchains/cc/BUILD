load("@rules_python//python:defs.bzl", "py_binary")

toolchain_type(
    name = "toolchain_type",
    visibility = ["//visibility:public"],
)

py_binary(
    name = "executor_host",
    srcs = [
        "executor_host.py",
    ],
    visibility = ["//visibility:public"],
)
