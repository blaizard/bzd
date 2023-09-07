load("@bzd_bundle//:defs.bzl", "bzd_bundle")
load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "client",
    srcs = [
        "client.py",
    ],
)

py_binary(
    name = "node",
    srcs = [
        "monitor.py",
        "power.py",
        "rest_server.py",
        "node.py",
    ],
    deps = [
        requirement("psutil"),
        requirement("requests"),
    ],
)

bzd_bundle(
    name = "node_manager",
    binary = ":node",
    tags = ["manual"],
)
