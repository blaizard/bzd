load("@bzd_bundle//:defs.bzl", "bzd_bundle")
load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@pip//:requirements.bzl", "requirement")
load("@rules_python//python:defs.bzl", "py_binary")
load("//apps/bootloader:defs.bzl", "bzd_bootloader_binary")

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
    name = "node_manager",
    srcs = [
        "monitor.py",
        "node_manager.py",
        "power.py",
        "rest_server.py",
    ],
    deps = [
        requirement("psutil"),
        "//apps/artifacts/api/python/node",
        "@bzd_python//bzd/http:client",
        "@bzd_python//bzd/sync:singleton",
        "@bzd_python//bzd/utils:scheduler",
    ],
)

bzd_config(
    name = "bootloader_config",
    data = [
        ":node_manager",
    ],
    values = {
        "application": "apps/node_manager/node_manager",
        "updatePath": "bzd/apps/node_manager",
    },
)

bzd_bootloader_binary(
    name = "with_bootloader",
    configs = {
        "//apps/bootloader:config": ":bootloader_config",
    },
)

bzd_bundle(
    name = "bundle",
    binary = ":with_bootloader",
    tags = ["manual"],
    visibility = ["//deployment:__subpackages__"],
)
