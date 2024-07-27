load("@bzd_bundle//:defs.bzl", "bzd_bundle")
load("@bzd_lib//config:defs.bzl", "bzd_config")
load("@rules_python//python:defs.bzl", "py_binary")

bzd_config(
    name = "config",
    srcs = [
        "config.default.json",
    ],
)

py_binary(
    name = "bootloader",
    srcs = [
        "bootloader.py",
        "manifest.py",
        "singleton.py",
    ],
    deps = [
        ":config",
        "//apps/artifacts/plugins/fs/release/python:release",
        "@bzd_python//bzd/utils:logging",
        "@bzd_python//bzd/utils:run",
    ],
)

bzd_bundle(
    name = "bundle",
    binary = ":bootloader",
    tags = ["manual"],
)
