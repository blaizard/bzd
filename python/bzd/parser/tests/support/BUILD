load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "support",
    srcs = glob([
        "*.py",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "@bzd_python//bzd/parser",
    ],
)
