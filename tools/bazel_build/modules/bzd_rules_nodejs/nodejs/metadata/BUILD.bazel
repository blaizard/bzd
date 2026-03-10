load("@rules_python//python:defs.bzl", "py_binary")

exports_files([
    "metadata_nodejs.json",
    "metadata_nodejs_web.json",
])

py_binary(
    name = "metadata",
    srcs = ["metadata.py"],
    visibility = ["//visibility:public"],
)
