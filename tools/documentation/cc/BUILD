load("@rules_python//python:defs.bzl", "py_binary")
load("//tools/documentation/cc:defs.bzl", "cc_documentation")

cc_documentation(
    name = "cc",
    srcs = [
        "//cc:documentation",
    ],
    data = glob([
        "**/*.py",
    ]),
    tags = ["manual"],
)

py_binary(
    name = "generator",
    srcs = glob([
        "**/*.py",
    ]),
    main = "documentation.py",
)
