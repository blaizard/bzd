load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "release",
    srcs = [
        "release.py",
    ],
    deps = [
        "@bzd_python//bzd/http:client",
    ],
)
