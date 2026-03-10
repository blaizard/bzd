load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "docker",
    srcs = [
        "docker.py",
    ],
    deps = [
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/utils:run",
        "@bzd_python//bzd/utils:scheduler",
        "@bzd_python//bzd/utils:to_yaml",
    ],
)
