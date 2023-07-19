load("@rules_python//python:defs.bzl", "py_binary")
load("@bzd_bundle//:defs.bzl", "bzd_bundle")

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
    name = "bzd_wol_server",
    binary = ":server",
    tags = ["manual"],
)
