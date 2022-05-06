load("@rules_python//python:defs.bzl", "py_binary", "py_library")

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

py_binary(
    name = "playground",
    srcs = [
        "playground.py",
    ],
    args = [
        "python/bzd/parser/scratch_pad.py",
    ],
    deps = [
        ":parser",
        "//python/bzd:env",
        "//python/bzd/utils:file",
    ],
)
