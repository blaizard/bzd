load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "sanitizer",
    srcs = [
        "sanitizer.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd_sanitizer:context",
        "@bzd_python//bzd/utils:run",
    ],
)
