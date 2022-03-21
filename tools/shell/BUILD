load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "setup",
    srcs = [
        "setup.py",
    ],
    deps = [
        "//python/bzd:env",
        "//tools/shell/sh:factory"
    ],
)


py_library(
    name = "factory",
    srcs = [
        "factory.py",
    ],
    deps = [
        "//python/bzd/template"
    ],
    visibility = [
        "//tools/shell:__subpackages__",
    ],
)
