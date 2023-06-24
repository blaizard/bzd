load("@rules_python//python:defs.bzl", "py_binary")
load("//tools/bazel_build/rules:bundle.bzl", "bzd_bundle")

py_binary(
    name = "client",
    srcs = [
        "client.py",
    ],
)

py_binary(
    name = "server",
    srcs = [
        "server.py",
    ],
)

bzd_bundle(
    name = "server_bundle",
    binary = ":server",
    tags = ["manual"],
)
