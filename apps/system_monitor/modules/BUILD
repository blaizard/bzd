load("@rules_python//python:defs.bzl", "py_library")
load("@pip//:requirements.bzl", "requirement")

py_library(
    name = "modules",
    srcs = [
        "py_psutil.py",
        "xsensors.py",
    ],
    visibility = [
        "//apps/system_monitor:__pkg__",
    ],
    deps = [
        "//python/bzd/utils:run",
        requirement("psutil"),
    ],
)
