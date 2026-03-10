load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "executor_qemu",
    srcs = [
        ":executor_qemu.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",
    ],
)
