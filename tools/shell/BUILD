load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_binary(
    name = "install",
    srcs = [
        "shell.py",
    ],
    main = "shell.py",
    deps = [
        ":shell",
    ],
)

py_library(
    name = "shell",
    srcs = [
        "shell.py",
    ],
    visibility = [
        "//tools/sanitizer/actions/shell:__pkg__",
    ],
    deps = [
        "//python/bzd:env",
        "//tools/shell:factory",
        "//tools/shell/sh:factory",
    ],
)

py_library(
    name = "factory",
    srcs = [
        "factory.py",
    ],
    visibility = [
        "//tools/shell:__subpackages__",
    ],
    deps = [
        "//python/bzd/template",
    ],
)
