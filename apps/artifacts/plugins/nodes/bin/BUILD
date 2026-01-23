load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "export",
    srcs = [
        "export.py",
    ],
    deps = [
        "//apps/artifacts/api/python/node",
        requirement("python-dateutil"),
    ],
)
