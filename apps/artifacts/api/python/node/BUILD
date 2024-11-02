load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_binary(
    name = "binary",
    srcs = [
        "binary.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":node",
    ],
)

py_library(
    name = "node",
    srcs = [
        "node.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//apps/artifacts/api:config",
        "//apps/artifacts/api/python:common",
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/http:utils",
        "@bzd_python//bzd/logging/handler",
    ],
)
