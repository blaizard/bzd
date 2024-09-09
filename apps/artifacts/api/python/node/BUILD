load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "node",
    srcs = [
        "node.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//apps/artifacts/api/python:common",
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/http:utils",
        "@bzd_python//bzd/utils:logging",
    ],
)
