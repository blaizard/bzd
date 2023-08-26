load("@rules_python//python:defs.bzl", "py_binary")

filegroup(
    name = "metadata_json",
    srcs = ["metadata.json"],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "metadata",
    srcs = ["metadata.py"],
    visibility = ["//visibility:public"],
)
