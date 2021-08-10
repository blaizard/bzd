load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "parser",
    srcs = glob([
        "*.py",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "//python:bzd",
        "//python/bzd/utils:dict",
        "//python/bzd/utils:result",
    ],
)
