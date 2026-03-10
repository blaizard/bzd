load("@bzd_python//:defs.bzl", "bzd_python_oci")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "sleep",
    srcs = [
        "sleep.py",
    ],
)

bzd_python_oci(
    name = "image_sleep",
    binary = ":sleep",
)
