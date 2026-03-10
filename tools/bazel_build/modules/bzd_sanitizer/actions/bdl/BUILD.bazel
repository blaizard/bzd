load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "bdl",
    srcs = [
        "bdl.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//python/bzd_sanitizer:worker",
        "@bzd_bdl//bdl:lib",
    ],
)
