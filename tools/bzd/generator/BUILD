load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "generator",
    srcs = glob([
        "**/*.py",
    ]),
    main = "generator.py",
    visibility = ["//visibility:public"],
)
