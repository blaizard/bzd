load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "docker_manager",
    srcs = [
        "docker_manager.py",
    ],
    deps = [
        "@bzd_python//bzd/http:parser",
    ],
)
