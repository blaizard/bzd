load("@rules_python//python:defs.bzl", "py_binary")
load("@bzd_bundle//:defs.bzl", "bzd_bundle")
load("@pip//:requirements.bzl", "requirement")

py_binary(
    name = "client",
    srcs = [
        "client.py",
    ],
)

py_binary(
    name = "server",
    srcs = [
        "monitor.py",
        "power.py",
        "rest_server.py",
        "server.py",
    ],
    deps = [
        requirement("psutil"),
    ],
)

bzd_bundle(
    name = "node_manager",
    binary = ":server",
    tags = ["manual"],
)
