load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "common",
    srcs = [
        "common.py",
        "//apps/artifacts/api:config.py",
    ],
    visibility = ["//apps/artifacts/api/python:__subpackages__"],
    deps = [
        "@bzd_python//bzd/logging",
    ],
)
