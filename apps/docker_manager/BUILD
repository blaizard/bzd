load("@bzd_python//:defs.bzl", "bzd_python_oci")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "docker_manager",
    srcs = [
        "docker_manager.py",
    ],
    deps = [
        "//apps/artifacts/api/python/node",
        "@bzd_python//bzd/http:parser",
        "@bzd_python//bzd/utils:scheduler",
    ],
)

bzd_python_oci(
    name = "image",
    base = "@oci_minimal",
    binary = ":docker_manager",
)
