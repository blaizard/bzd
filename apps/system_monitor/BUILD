load("@rules_python//python:defs.bzl", "py_binary")
load("@bzd_bundle//:defs.bzl", "bzd_bundle")

py_binary(
    name = "system_monitor",
    srcs = [
        "system_monitor.py",
    ],
    deps = [
        "//apps/system_monitor/modules",
        "//python/bzd/utils:dict",
    ],
)

bzd_bundle(
    name = "bzd_system_monitor",
    binary = ":system_monitor",
    tags = ["manual"],
)
