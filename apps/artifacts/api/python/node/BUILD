load("@rules_python//python:defs.bzl", "py_binary", "py_library")

py_binary(
    name = "binary",
    srcs = [
        "binary.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":node",
        "@bzd_python//bzd/logging",
    ],
)

py_library(
    name = "node",
    srcs = [
        "node.py",
        "//apps/artifacts/api:config.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//apps/artifacts/api/python:common",
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/http:utils",
        "@bzd_python//bzd/logging",
        "@bzd_python//bzd/logging/handler",
    ],
)
