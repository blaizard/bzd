load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "build",
    srcs = [
        "build.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",
    ],
)
