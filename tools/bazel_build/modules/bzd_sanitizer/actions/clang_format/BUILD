load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "clang_format",
    srcs = [
        "clang_format.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/utils:run",
        "@bzd_sanitizer//python/bzd_sanitizer:worker",
    ],
)
