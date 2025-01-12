load("@rules_python//python:defs.bzl", "py_binary")

exports_files([
    "doxygen.config",
])

py_binary(
    name = "doxygen_parser",
    srcs = ["doxygen_parser.py"],
    visibility = ["//doc:__pkg__"],
)
