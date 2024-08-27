load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "common",
    srcs = [
        "common.py",
    ],
    visibility = ["//apps/artifacts/api/python:__subpackages__"],
    deps = [
        "//apps/artifacts/api:config",
        "@bzd_python//bzd/utils:logging",
    ],
)
