load("@bzd_python//:defs.bzl", "bzd_python_binary_library")
load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "run",
    srcs = ["run.py"],
    imports = [".."],
    visibility = ["//visibility:public"],
    deps = [
        ":bin",
        "@bzd_python//bzd/utils:run",
    ],
)

bzd_python_binary_library(
    name = "bin",
    executables = {
        "//:crane": "crane",
    },
    visibility = ["//:__subpackages__"],
)
