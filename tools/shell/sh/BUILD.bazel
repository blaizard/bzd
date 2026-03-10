load("@rules_python//python:defs.bzl", "py_library")

py_library(
    name = "factory",
    srcs = [
        "factory.py",
    ],
    data = glob(
        [
            "srcs/**/*",
        ],
    ),
    visibility = [
        "//tools/shell:__pkg__",
    ],
    deps = [
        "//tools/shell:factory",
    ],
)
