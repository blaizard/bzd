load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "validation",
    srcs = glob([
        "*.py",
        "constraints/**/*.py",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//python:bzd",
        "//python/bzd/utils:decorators",
    ],
)
