load("@rules_python//python:defs.bzl", "py_binary")

py_binary(
    name = "build",
    srcs = [
        "build.py",
    ],
    deps = [
        "@bzd_python//bzd/command_extractor:gcc",
        "@bzd_python//bzd/utils:run",
    ],
)
