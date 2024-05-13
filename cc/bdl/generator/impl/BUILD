load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "visitor",
    srcs = glob([
        "**/*.py",
    ]),
    data = glob([
        "template/*.btl",
    ]),
    visibility = [
        "//cc/bdl/generator:__subpackages__",
    ],
)
