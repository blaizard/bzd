load("@bzd_bundle//:defs.bzl", "bzd_bundle")
load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "client",
    srcs = [
        "client.py",
    ],
    visibility = [
        "//visibility:public",
    ],
)

py_binary(
    name = "node",
    srcs = [
        "monitor.py",
        "node.py",
        "power.py",
        "rest_server.py",
        "singleton.py",
    ],
    deps = [
        requirement("psutil"),
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/utils:scheduler",
    ],
)

bzd_bundle(
    name = "node_manager",
    binary = ":node",
    tags = ["manual"],
    visibility = ["//deployment:__subpackages__"],
)
