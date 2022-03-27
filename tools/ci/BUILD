load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "factory",
    srcs = [
        "factory.py",
    ],
    data = [
        "stages.json",
    ],
    visibility = [
        "//tools/ci:__subpackages__",
    ],
    deps = [
        "//python/bzd/template",
    ],
)
