load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "release",
    srcs = [
        "release.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//apps/artifacts/api/python:common",
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/logging",
    ],
)

py_library(
    name = "mock",
    srcs = [
        "mock.py",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":release",
    ],
)
