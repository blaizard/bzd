load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_library(
    name = "yaml",
    srcs = glob([
        "yaml/*.py",
    ]),
    visibility = ["//visibility:private"],
)

py_library(
    name = "formats",
    srcs = glob([
        "formats/*.py",
    ]),
    visibility = ["//visibility:private"],
)

py_binary(
    name = "generator",
    srcs = glob([
        "*.py",
    ]),
    main = "generator.py",
    visibility = ["//visibility:public"],
    deps = [
        ":formats",
        ":yaml",
    ],
)
