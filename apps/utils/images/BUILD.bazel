load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "images",
    srcs = [
        "converter.py",
        "images.py",
        "pillow.py",
        "rawpy.py",
    ],
    deps = [
        requirement("pillow"),
        requirement("rawpy"),
    ],
)
