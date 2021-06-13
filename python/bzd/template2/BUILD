load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "template2",
    srcs = glob([
        "*.py",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//python:bzd",
        "//python/bzd/parser",
    ],
)
