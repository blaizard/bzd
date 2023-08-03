load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "sanitizer",
    srcs = [
        "sanitizer.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",
        "@bzd_sanitizer//lib:context",
    ],
)
